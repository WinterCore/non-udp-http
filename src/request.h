#ifndef REQUEST_H
#define REQUEST_H
#include "connection.h"
#include "string.h"

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

typedef struct HttpHeaders {
    size_t len;
    char **keys;
    char **values;
} HttpHeaders;

void read_pathname_and_query_string(
    Arena *arena, 
    const char *request_target,
    char **out_pathname,
    char **out_query_string
);

int http_headers_add(HttpHeaders *headers, char *key, char *value);
int http_headers_delete(HttpHeaders *headers, const char *key, char **deleted_value);
int http_headers_clear(HttpHeaders *headers);
int http_headers_find(HttpHeaders *headers, const char *key, char **out_value);
bool http_headers_iter(HttpHeaders *headers, size_t *index, char **out_key, char **out_value);

typedef struct HttpQueryParams {
    size_t len;
    Earring **keys;
    Earring **values;
} HttpQueryParams;

int http_query_string_parse(Arena *arena, const char *query_string, HttpQueryParams *out_params);

int http_query_params_add(Arena *arena, HttpQueryParams *params, const char *key, const char *value);
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

typedef struct HttpResponse {
    int status_code;
    HttpHeaders headers;
    uint8_t *body;
    size_t body_length;
} HttpResponse;

HttpRequest create_http_request(Arena *arena);
HttpRequestParseResult parse_http_request(HttpConnection *connection, HttpRequest *out_request);

#endif
