#ifndef AIDS_H
#define AIDS_H

#include <stdio.h>
#include <stdlib.h>

#define MIN(a,b) \
    ({ __typeof__(a) _a = (a); \
       __typeof__(b) _b = (b); \
       _a < _b ? _a : _b; })

#define MAX(a,b) \
    ({ __typeof__(a) _a = (a); \
       __typeof__(b) _b = (b); \
       _a > _b ? _a : _b; })

#define DEBUG_PRINT_ENABLED 1
#define DEBUG_PRINT(...) \
    do { if (DEBUG_PRINT_ENABLED) fprintf(stderr, __VA_ARGS__); } while (0)

#define PANIC(fmt, ...) do { \
    fprintf(stderr, "PANIC at %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
    exit(EXIT_FAILURE); \
} while (0)

#endif
