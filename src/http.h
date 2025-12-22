#ifndef HTTP_H
#define HTTP_H
#include "arena.h"
#include <stddef.h>
#include <stdbool.h>

#define MAX_HEADERS 100

typedef struct HttpHeaders {
    size_t len;
    const char **keys;
    const char **values;
} HttpHeaders;

HttpHeaders create_http_headers(Arena *arena);

int http_headers_add(HttpHeaders *headers, const char *key, const char *value);
int http_headers_delete(HttpHeaders *headers, const char *key, char **deleted_value);
int http_headers_clear(HttpHeaders *headers);
int http_headers_find(HttpHeaders *headers, const char *key, char **out_value);
bool http_headers_iter(HttpHeaders *headers, size_t *index, char **out_key, char **out_value);
#endif
