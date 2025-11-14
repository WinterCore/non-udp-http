#include <string.h>

#include "request.h"
#include "aids.h"
#include "arena.h"
#include "buffer.h"

// The real limit is 8192 - strlen("\r\n") because the buffer reader copies the delimiter too but doesn't count it
#define MAX_STATUS_LINE_LENGTH 8192

HttpRequestParseStatus parse_http_request(HttpConnection *connection, HttpRequest *out_request) {
    {
        uint8_t *raw_status_line = arena_alloc(connection->arena, MAX_STATUS_LINE_LENGTH);
        // Parse status line
        int bytes_read = fd_buffer_reader_read_until(&connection->reader, raw_status_line, "\r\n", MAX_STATUS_LINE_LENGTH);

        DEBUG_PRINT("Read %d bytes for status line\n", bytes_read);

        if (bytes_read == 0) {
            return HTTP_PARSE_INCOMPLETE_REQUEST;
        }

        if (bytes_read == -1) {
            return HTTP_PARSE_UNKNOWN_READ_ERROR;
        }

        if (bytes_read == -2) {
            return HTTP_PARSE_STATUS_LINE_TOO_LONG;
        }

        char *protocol = strtok((char *) raw_status_line, " ");
        char *status_code = strtok(NULL, " ");
        char *reason_phrase = strtok(NULL, "\r\n"); // Handles possible line endings

        printf("Protocol: %s\n", protocol);
        printf("Status Code: %s\n", status_code);
        printf("Reason Phrase: %s\n", reason_phrase);
    }

    return HTTP_PARSE_OK;
}

