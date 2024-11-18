#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include "./hector.h"
#include "./helpers.h"

#define INITIAL_CAPACITY 10
#define GROW_FACTOR 2
#define SHRINK_THRESHOLD (1.0f / 4.0f)
#define SHRINK_FACTOR (0.5f) // SHRINK FACTOR SHOULD BE BIGGER THAN SHRINK_THRESHOLD

Hector hector_create(size_t elem_size) {
    uint8_t *data = malloc(INITIAL_CAPACITY * elem_size);

    if (data == NULL) {
        PANIC("Not enough memory");
    }

    Hector hector = {
        .capacity = INITIAL_CAPACITY,
        .len = 0,
        .data = data,
        .elem_size = elem_size,
    };

    return hector;
}

static void grow_if_full(Hector *hector) {
    if (hector->len < hector->capacity) {
        return;
    }
    
    size_t new_capacity = hector->capacity * GROW_FACTOR;
    hector->capacity = new_capacity;
    hector->data = realloc(hector->data, new_capacity * hector->elem_size);
}

static void shrink_if_needed(Hector *hector) {
    if ((float) hector->len / (float) hector->capacity > SHRINK_THRESHOLD) {
        return;
    }
    
    size_t new_capacity = MAX(
        (float) hector->capacity * SHRINK_FACTOR,
        INITIAL_CAPACITY
    );
    hector->capacity = new_capacity;
    hector->data = realloc(hector->data, new_capacity * hector->elem_size);
}

void hector_push(Hector *hector, void *value) {
    grow_if_full(hector);
    memcpy(&hector->data[hector->len * hector->elem_size], value, hector->elem_size);

    hector->len += 1;
}

void hector_pop(Hector *hector) {
    if (hector->len > 0) {
        hector->len -= 1;
        shrink_if_needed(hector);
    }
}

void *hector_get(Hector *hector, size_t index) {
    return (void *) &hector->data[index * hector->elem_size];
}

size_t hector_len(Hector *hector) {
    return hector->len;
}

void hector_destroy(Hector *hector) {
    free(hector->data);
}
