#ifndef HTTP_H
#define HTTP_H

#include "../hector.h"

struct HttpRequest {
    char *method;
    char *pathname;
    char *version;

    Hector headers;
    
    size_t body_len;
    uint8_t *body;
};

struct HttpHeader {
    char *key;
    char *value;
};

#endif
