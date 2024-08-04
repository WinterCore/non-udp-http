#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>
#include <stdlib.h>

#define DEBUG_PRINTF(fmt, ...) \
    fprintf(stderr, "-----DEBUG----> %s:%d:%s(): " fmt "\n\n", __FILE__, \
                        __LINE__, __func__, __VA_ARGS__); \
    fflush(stderr); \

#define DEBUG_PRINT(str) \
    DEBUG_PRINTF(str "%s", "");

#define PANIC(str) \
    perror(str); \
    exit(EXIT_FAILURE); \

#endif
