#ifndef REQUEST_H
#define REQUEST_H
#include "connection.h"
#include "earring.h"
#include "http.h"

typedef enum HttpRequestParseResult {
    HTTP_PARSE_OK = 0,
    HTTP_PARSE_STATUS_LINE_TOO_LONG,
    HTTP_PARSE_MALFORMED_STATUS_LINE,
    HTTP_PARSE_MALFORMED_HEADER_LINE,
    HTTP_PARSE_HEADER_TOO_LONG,
    HTTP_PARSE_TOO_MANY_HEADERS,
    HTTP_PARSE_INCOMPLETE_REQUEST,
    HTTP_PARSE_UNKNOWN_READ_ERROR,
} HttpRequestParseResult;

void read_pathname_and_query_string(
    Arena *arena, 
    const char *request_target,
    char **out_pathname,
    char **out_query_string
);

typedef struct HttpQueryParams {
    size_t len;
    Earring **keys;
    Earring **values;
} HttpQueryParams;

// TODO: Return parse result
void parse_http_query_string(Arena *arena, const char *query_string, HttpQueryParams *out_params);

int http_query_params_add(
    Arena *arena,
    HttpQueryParams *params,
    const char *key,
    size_t key_len,
    const char *value,
    size_t value_len
);
int http_query_params_clear(Arena *arena, HttpQueryParams *params);
int http_query_params_delete(Arena *arena, HttpQueryParams *params, const char *key, const char *value);

typedef struct HttpRequest {
    char *http_version;
    char *method;
    char *request_target; // Pathname + query string
    char *pathname; // Pathname only
    char *query_string; // Raw query string

    HttpHeaders headers;
    HttpQueryParams query_params;
} HttpRequest;

HttpRequest create_http_request(Arena *arena);
HttpRequestParseResult parse_http_request(HttpConnection *connection, HttpRequest *out_request);

#endif
