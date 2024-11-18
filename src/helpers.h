#ifndef HELPERS_H
#define HELPERS_H
#include <stdio.h>

#define PANIC(msg) \
    fprintf(stderr, "PANIC(%s:%d): " msg, __FILE__, __LINE__); \
    exit(EXIT_FAILURE);

#define MAX(a,b) ((a) > (b) ? a : b)
#define MIN(a,b) ((a) < (b) ? a : b)

#endif
