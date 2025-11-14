#ifndef STRING_H
#define STRING_H
#include "arena.h"
#include <stddef.h>

typedef struct Earring {
    size_t length;
    size_t max_length;

    char data[];
} Earring;


Earring *string_create(Arena *arena, size_t max_length);
int string_set(Earring *er, const char *str, size_t str_length);
int string_append(Earring *er, const char *suffix, size_t suffix_length);
int append_char(Earring *er, char c);
int string_prepend(Earring *er, const char *prefix, size_t prefix_length);

#endif
