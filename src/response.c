#include "./response.h"
#include "buffer.h"
#include "http.h"
#include <string.h>

HttpResponse create_http_response(Arena *arena) {
    HttpResponse response;

    response.http_version = "HTTP/1.1";

    response.status_code = 200;
    response.reason_phrase = "OK";

    response.headers = create_http_headers(arena);
    response.state = HTTP_RESPONSE_STATE_INITIAL;

    return response;
}

void http_response_set_status(HttpResponse *response, int status_code, const char *reason_phrase) {
    response->status_code = status_code;
    response->reason_phrase = (char *)reason_phrase;
}

int http_response_add_header(HttpResponse *response, const char *key, const char *value) {
    // TODO: Add limit on the header line key+value
    return http_headers_add(&response->headers, key, value);
}

int http_response_serialize_status_line(HttpResponse *response, BufferWriter *writer) {
    // HTTP/1.1 200 OK\r\n
    size_t start_bytes = writer->position;

    // Write http version
    if (buffer_write_str(writer, response->http_version) < 0) return -1;

    // Write a space
    if (buffer_write_char(writer, ' ') < 0) return -1;

    // Write the status code
    if (buffer_write_int_as_string(writer, response->status_code) < 0) return -1;

    // Write another space
    if (buffer_write_char(writer, ' ') < 0) return -1;

    // Write the status text
    if (buffer_write_str(writer, response->reason_phrase) < 0) return -1;

    // Write \r\n
    if (buffer_write_str(writer, "\r\n") < 0) return -1;

    return writer->position - start_bytes;
}

int http_response_serialize_headers(HttpResponse *response, BufferWriter *writer) {
    char *header_key = NULL;
    char *header_value = NULL;
    size_t i = 0;

    size_t start_bytes = writer->position;

    // TODO: Research if we need to have certain headers automatically populated and sent the by the server
    while (http_headers_iter(&response->headers, &i, &header_key, &header_value)) {
        // Write header key
        if (buffer_write_str(writer, header_key) < 0) return -1;

        // Write header separator
        if (buffer_write_str(writer, ": ") < 0) return -1;

        // Write header value
        if (buffer_write_str(writer, header_value) < 0) return -1;

        // Write \r\n
        if (buffer_write_str(writer, "\r\n") < 0) return -1;

    }

    return writer->position - start_bytes;
}

int http_response_serialize_head(HttpResponse *response, BufferWriter *writer) {
    // Write status line
    int status_line_bytes_written = http_response_serialize_status_line(response, writer);

    if (status_line_bytes_written < 0) {
        return status_line_bytes_written;
    }

    // Write headers
    int headers_bytes_written = http_response_serialize_headers(response, writer);
    
    if (headers_bytes_written < 0) {
        return headers_bytes_written;
    }

    // Write body separator \r\n
    if (buffer_write_str(writer, "\r\n") < 0) return -1;
    
    return status_line_bytes_written + headers_bytes_written + 2;
}
