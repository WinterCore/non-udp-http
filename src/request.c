#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "request.h"
#include "aids.h"
#include "arena.h"
#include "buffer.h"
#include "earring.h"

// The real limit is 8192 - strlen("\r\n") because the buffer reader copies the delimiter too but doesn't count it
#define MAX_STATUS_LINE_LENGTH 8192

#define MAX_HEADER_LINE_LENGTH 8192
#define MAX_HEADERS 100
#define MAX_QUERY_PARAMS 500

/**
 * Parses the HTTP status line from the connection's buffer reader.
 * Fills out_request with method, pathname, and http_version on success.
 */
HttpRequestParseResult parse_request_line(HttpConnection *connection, HttpRequest *out_request) {
    uint8_t *raw_status_line = arena_alloc(connection->arena, MAX_STATUS_LINE_LENGTH);

    int raw_status_line_len;
    // Parse status line
    int raw_status_line_read_until_result = fd_buffer_reader_read_until(&connection->reader, raw_status_line, "\r\n", MAX_STATUS_LINE_LENGTH, &raw_status_line_len);

    if (raw_status_line_read_until_result != BUFFER_READER_READ_OK) {
        // Handle errors first
        if (raw_status_line_read_until_result == BUFFER_READER_READ_FD_ERROR) {
            return HTTP_PARSE_UNKNOWN_READ_ERROR;
        }

        if (raw_status_line_read_until_result == BUFFER_READER_READ_MAX_BYTES_EXCEEDED) {
            return HTTP_PARSE_STATUS_LINE_TOO_LONG;
        }

        if (raw_status_line_read_until_result == BUFFER_READER_READ_EOF_BEFORE_DELIMITER) {
            return HTTP_PARSE_INCOMPLETE_REQUEST;
        }
        
        PANIC("Unhandled fd_buffer_reader_read_until result: %d", raw_status_line_read_until_result);
    }

    // Check length
    if (raw_status_line_len == 0) {
        return HTTP_PARSE_INCOMPLETE_REQUEST;
    }

    // TODO: Refactor status line parts into separate functions
    // Parse method
    char *protocol_end = memchr(raw_status_line, ' ', raw_status_line_len);

    if (protocol_end == NULL) {
        return HTTP_PARSE_MALFORMED_STATUS_LINE;
    }

    size_t method_len = protocol_end - (char *)raw_status_line;

    if (method_len == 0) {
        return HTTP_PARSE_MALFORMED_STATUS_LINE;
    }

    // Parse path
    char *request_target_start = protocol_end + 1;
    char *request_target_end = memchr(request_target_start, ' ', raw_status_line_len - method_len - 1);

    if (request_target_end == NULL) {
        return HTTP_PARSE_MALFORMED_STATUS_LINE;
    }

    size_t request_target_len = request_target_end - request_target_start;

    // TODO: Make sure pathname doesn't contain illegal (control) characters
    // Validate valid origin-form path
    if (request_target_len == 0 || request_target_start[0] != '/') {
        return HTTP_PARSE_MALFORMED_STATUS_LINE;
    }

    // Parse version
    char *version_start = request_target_end + 1;
    size_t version_len = raw_status_line_len - method_len - request_target_len - 2;

    // validate version
    if (version_len < 5 || (memcmp(version_start, "HTTP/", 5) != 0)) {
        return HTTP_PARSE_MALFORMED_STATUS_LINE;
    }

    char *method = arena_alloc(connection->arena, method_len + 1);
    char *request_target = arena_alloc(connection->arena, request_target_len + 1);
    char *http_version = arena_alloc(connection->arena, version_len + 1);

    memcpy(method, raw_status_line, method_len);
    method[method_len] = '\0';

    memcpy(request_target, request_target_start, request_target_len);
    request_target[request_target_len] = '\0';

    memcpy(http_version, version_start, version_len);
    http_version[version_len] = '\0';

    out_request->method = method;
    out_request->request_target = request_target;
    out_request->http_version = http_version;

    return HTTP_PARSE_OK;
}

HttpRequestParseResult parse_http_headers(HttpConnection *connection, HttpRequest *out_request) {
    while (true) {
        char *header_line = arena_alloc(connection->arena, MAX_HEADER_LINE_LENGTH);

        int header_line_len;

        BufferReaderReadResult header_line_read_until_result = fd_buffer_reader_read_until(
            &connection->reader,
            (uint8_t *) header_line,
            "\r\n",
            MAX_HEADER_LINE_LENGTH,
            &header_line_len
        );

        if (header_line_read_until_result != BUFFER_READER_READ_OK) {
            // Handle errors first
            if (header_line_read_until_result == BUFFER_READER_READ_FD_ERROR) {
                return HTTP_PARSE_UNKNOWN_READ_ERROR;
            }

            if (header_line_read_until_result == BUFFER_READER_READ_MAX_BYTES_EXCEEDED) {
                return HTTP_PARSE_HEADER_TOO_LONG;
            }

            if (header_line_read_until_result == BUFFER_READER_READ_EOF_BEFORE_DELIMITER) {
                return HTTP_PARSE_INCOMPLETE_REQUEST;
            }
            
            PANIC("Unhandled fd_buffer_reader_read_until result: %d", header_line_read_until_result);
        }

        /**
         * Finished reading headers
         * An empty line indicates the end of headers because of the two consequtive CRLFs at the end of headers
         * e.g.
         * Content-Type: text/html\r\n
         * \r\n
         * Body starts here
         */
        if (header_line_len == 0) {
            break;
        }

        assert(header_line_len >= 0 && "Unhandled read_until result");

        // Parse header key and value
        char *separator = memchr(header_line, ':', header_line_len);

        if (separator == NULL) {
            return HTTP_PARSE_MALFORMED_HEADER_LINE;
        }

        size_t key_len = separator - header_line;
        size_t value_len = header_line_len - key_len - 1; // -1 for the ':'

        char *key = arena_alloc(connection->arena, key_len + 1);
        char *value = arena_alloc(connection->arena, value_len + 1);

        memcpy(key, header_line, key_len);
        key[key_len] = '\0';

        // Skip leading whitespace in value
        char *value_start = separator + 1;
        while (value_len > 0 && (*value_start == ' ' || *value_start == '\t')) {
            value_start += 1;
            value_len -= 1;
        }
        
        memcpy(value, value_start, value_len);
        value[value_len] = '\0';

        // TODO: Check if limit is reached
        if (http_headers_add(&out_request->headers, key, value) == -1) {
            return HTTP_PARSE_TOO_MANY_HEADERS;
        }
    }

    return HTTP_PARSE_OK;
}

HttpRequest create_http_request(Arena *arena) {
    HttpRequest request;

    request.method = NULL;
    request.request_target = NULL;
    request.http_version = NULL;

    request.headers.len = 0;
    request.headers.keys = arena_alloc(arena, sizeof(char *) * MAX_HEADERS);
    request.headers.values = arena_alloc(arena, sizeof(char *) * MAX_HEADERS);

    request.query_params.len = 0;
    request.query_params.keys = arena_alloc(arena, sizeof(Earring *) * MAX_QUERY_PARAMS);
    request.query_params.values = arena_alloc(arena, sizeof(Earring *) * MAX_QUERY_PARAMS);

    return request;
}

/**
 * Splits the request target into pathname and query string.
 */
void read_pathname_and_query_string(
    Arena *arena, 
    const char *request_target,
    char **out_pathname,
    char **out_query_string
) {
    char *query_start = strchr(request_target, '?');
    size_t request_target_len = strlen(request_target);

    if (query_start == NULL) {
        // No query string
        *out_pathname = arena_alloc(arena, request_target_len + 1);
        strncpy(*out_pathname, request_target, request_target_len);
        (*out_pathname)[request_target_len] = '\0';

        *out_query_string = NULL;

        return;
    }

    size_t pathname_len = query_start - request_target;
    size_t query_string_len = request_target_len - pathname_len - 1; // -1 for '?'

    *out_pathname = arena_alloc(arena, pathname_len + 1);
    *out_query_string = arena_alloc(arena, query_string_len + 1);

    strncpy(*out_pathname, request_target, pathname_len);
    (*out_pathname)[pathname_len] = '\0';

    strncpy(*out_query_string, query_start + 1, query_string_len);
    (*out_query_string)[query_string_len] = '\0';
}

/**
 * Parses the query string into key-value pairs.
 */
void parse_http_query_string(Arena *arena, const char *query_string, HttpQueryParams *out_params) {
    if (query_string == NULL) {
        return;
    }

    size_t query_string_len = strlen(query_string);

    size_t i = 0;

    // TODO: Make sure to check for the limit of MAX_QUERY_PARAMS

    while (i < query_string_len) {
        // Find key
        size_t key_start = i;
        size_t key_len = 0;


        // TODO: Check for illegal characters in key & value

        while (i < query_string_len && query_string[i] != '=' && query_string[i] != '&') {
            i += 1;
            key_len += 1;
        }

        // Consume '='
        while (i < query_string_len && query_string[i] == '=') {
            // There should be only one '=' but we tolerate multiple '=' for robustness
            i += 1;
        }

        size_t value_start = i;
        size_t value_len = 0;
        
        while (i < query_string_len && query_string[i] != '&') {
            i += 1;
            value_len += 1;
        }

        // Add key-value pair
        http_query_params_add(
            arena,
            out_params,
            key_len > 0 ? &query_string[key_start] : NULL,
            key_len,
            value_len > 0 ? &query_string[value_start] : NULL,
            value_len
        );

        // Consume '&'
        while (i < query_string_len && query_string[i] == '&') {
            // There should be only one '&' but we tolerate multiple '&' for robustness
            i += 1;
        }
    }
}


/**
 * Parses an HTTP request from the given connection.
 * Fills out_request with the parsed request details.
 */
HttpRequestParseResult parse_http_request(HttpConnection *connection, HttpRequest *out_request) {
    // Parse status line
    HttpRequestParseResult status_line_parse_result = parse_request_line(connection, out_request);

    if (status_line_parse_result != HTTP_PARSE_OK) {
        return status_line_parse_result;
    }

    HttpRequestParseResult headers_parse_result = parse_http_headers(connection, out_request);

    if (headers_parse_result != HTTP_PARSE_OK) {
        return headers_parse_result;
    }

    // TOOD: Validate headers. HTTP/1.1 requires Host, Content-Length and maybe Content-Type

    // Split pathname and query string
    read_pathname_and_query_string(
        connection->arena,
        out_request->request_target,
        &out_request->pathname,
        &out_request->query_string
    );

    parse_http_query_string(connection->arena, out_request->query_string, &out_request->query_params);

    return HTTP_PARSE_OK;
}

int http_headers_add(HttpHeaders *headers, char *key, char *value) {
    if (headers->len >= MAX_HEADERS) {
        return -1;
    }

    int index = headers->len;

    headers->keys[index] = key;
    headers->values[index] = value;
    headers->len += 1;

    return index;
}

// TODO: Should delete all occurences of the key
int http_headers_delete(HttpHeaders *headers, const char *key, char **deleted_value) {
    int index = http_headers_find(headers, key, deleted_value);

    if (index > -1) {
        size_t items_to_move = headers->len - index - 1;

        if (items_to_move > 0) {
            memcpy(headers->keys[index], headers->keys[index + 1], sizeof(char *) * items_to_move);
            memcpy(headers->values[index], headers->values[index + 1], sizeof(char *) * items_to_move);
        }

        headers->len -= 1;
    }

    return index;
}

int http_headers_clear(HttpHeaders *headers) {
    int len = headers->len;
    headers->len = 0;

    return len;
}

int http_headers_find(HttpHeaders *headers, const char *key, char **out_value) {
    for (size_t i = 0; i < headers->len; i += 1) {
        if (strcmp(headers->keys[i], key) == 0) {
            *out_value = headers->values[i];

            return i;
        }
    }

    *out_value = NULL;

    return -1;
}

bool http_headers_iter(HttpHeaders *headers, size_t *index, char **out_key, char **out_value) {
    if (*index >= headers->len) {
        return false;
    }

    *out_key = headers->keys[*index];
    *out_value = headers->values[*index];
    *index += 1;

    return true;
}

/**
 * Adds a key-value pair to the query parameters.
 * If key or value is NULL, it represents an empty key or value respectively.
 *
 * TODO: Parse and decode percent-encoded characters, and maybe discard illegal characters.
 */
int http_query_params_add(
    Arena *arena,
    HttpQueryParams *params,
    const char *key,
    size_t key_len,
    const char *value,
    size_t value_len
) {
    if (params->len >= MAX_QUERY_PARAMS) {
        return -1;
    }

    int index = params->len;

    Earring *key_er = NULL;
    Earring *value_er = NULL;

    if (key != NULL) {
        key_er = earring_create(arena, key_len);
        earring_set(key_er, key, key_len);
    }

    if (value != NULL) {
        value_er = earring_create(arena, value_len);
        earring_set(value_er, value, value_len);
    }

    params->keys[index] = key_er;
    params->values[index] = value_er;
    params->len += 1;

    return index;
}
