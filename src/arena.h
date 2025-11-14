#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>
#include <stdint.h>

#define ARENA_PAGE_SIZE 4096

typedef struct Arena {
    uint8_t *data;
    size_t size;
    size_t current;
    struct Arena *next;
} Arena;

Arena *arena_create();

void *arena_alloc(Arena *arena, size_t size);
void arena_reset(Arena *arena);
void arena_reset(Arena *arena);
void arena_destroy(Arena *arena);

#endif
