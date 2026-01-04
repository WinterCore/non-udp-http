#ifndef RESPONSE_H
#define RESPONSE_H
#include "arena.h"
#include "buffer.h"
#include "http.h"

typedef enum HttpResponseState {
    HTTP_RESPONSE_STATE_INITIAL,
    HTTP_RESPONSE_STATE_HEADERS_SENT,
    HTTP_RESPONSE_STATE_BODY_SENT,
    HTTP_RESPONSE_STATE_CLOSED,
} HttpResponseState;

typedef struct HttpResponse {
    char *http_version;

    int status_code;
    char *reason_phrase;

    HttpHeaders headers;

    HttpResponseState state;
} HttpResponse;

/**
 * Creates a new HttpResponse with default values.
 */
HttpResponse create_http_response(Arena *arena);

/**
 * Sets the status code and reason phrase of the response.
 */
void http_response_set_status(HttpResponse *response, int status_code, const char *reason_phrase);

/**
 * Adds a header to the response.
 * Returns the index of the added header, or -1 on failure.
 */
int http_response_add_header(HttpResponse *response, const char *key, const char *value);

/**
 * Serialize the status line (e.g., "HTTP/1.1 200 OK\r\n") into buffer.
 * Returns bytes written, or -1 if buffer too small.
 */
int http_response_serialize_status_line(HttpResponse *response, BufferWriter *writer);

/**
 * Serialize all headers into buffer (each header ends with \r\n).
 * Returns bytes written, or -1 if buffer too small.
 */
int http_response_serialize_headers(HttpResponse *response, BufferWriter *writer);

/**
 * Serialize status line and headers into buffer (ends with \r\n\r\n).
 * Returns bytes written, or -1 if buffer too small.
 */
int http_response_serialize_head(HttpResponse *response, BufferWriter *writer);

#endif
