#ifndef EARRING_H
#define EARRING_H
#include <stddef.h>
#include <stdio.h>

#include "arena.h"

typedef struct Earring {
    size_t length;
    size_t max_length;

    char data[];
} Earring;

Earring *earring_create(Arena *arena, size_t max_length);
int earring_set(Earring *er, const char *str, size_t str_length);
int earring_append(Earring *er, const char *suffix, size_t suffix_length);
int earring_append_char(Earring *er, char c);
int earring_string_prepend(Earring *er, const char *prefix, size_t prefix_length);
int earring_write(FILE *stream, Earring *er);

#endif
