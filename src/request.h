#ifndef REQUEST_H
#define REQUEST_H
#include "connection.h"

typedef enum HttpRequestParseStatus {
    HTTP_PARSE_OK = 0,
    HTTP_PARSE_STATUS_LINE_TOO_LONG,
    HTTP_PARSE_HEADER_TOO_LONG,
    HTTP_PARSE_INCOMPLETE_REQUEST,
    HTTP_PARSE_UNKNOWN_READ_ERROR,
} HttpRequestParseStatus;

typedef struct HttpRequest {
    char *method;
    char *pathname;
    char *http_version;
} HttpRequest;


HttpRequestParseStatus parse_http_request(HttpConnection *connection, HttpRequest *out_request);

#endif
