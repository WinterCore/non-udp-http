#ifndef HECTOR_H
#define HECTOR_H
#include <stdlib.h>
#include <inttypes.h>

typedef struct Hector {
    size_t capacity;
    size_t len;
    size_t elem_size;
    
    uint8_t *data;
} Hector;

Hector hector_create(size_t elem_size);
void hector_push(Hector *hector, void *value);
void hector_pop(Hector *hector);
void *hector_get(Hector *hector, size_t index);
size_t hector_len(Hector *hector);

void hector_destroy(Hector *hector);

#endif

