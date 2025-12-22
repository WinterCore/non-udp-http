#include <string.h>

#include "http.h"

HttpHeaders create_http_headers(Arena *arena) {
    HttpHeaders headers;

    headers.len = 0;
    headers.keys = arena_alloc(arena, sizeof(char *) * MAX_HEADERS);
    headers.values = arena_alloc(arena, sizeof(char *) * MAX_HEADERS);

    return headers;
}

int http_headers_add(HttpHeaders *headers, const char *key, const char *value) {
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
            memcpy((char *) headers->keys[index], headers->keys[index + 1], sizeof(char *) * items_to_move);
            memcpy((char *) headers->values[index], headers->values[index + 1], sizeof(char *) * items_to_move);
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
            *out_value = (char *) headers->values[i];

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

    *out_key = (char *) headers->keys[*index];
    *out_value = (char *) headers->values[*index];
    *index += 1;

    return true;
}
