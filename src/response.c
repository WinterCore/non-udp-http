#include "./response.h"

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

int http_response_add_header(HttpResponse *response, const char *name, const char *value) {
    return http_headers_add(&response->headers, name, value);
}
