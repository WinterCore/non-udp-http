#include "earring.h"
#include "arena.h"
#include "stdlib.h"
#include <stdio.h>

Earring *earring_create(Arena *arena, size_t max_length) {
    Earring *str = (Earring *)arena_alloc(arena, sizeof(Earring) + max_length * sizeof(char));

    if (!str) {
        return NULL;
    }

    str->length = 0;
    str->max_length = max_length;

    return str;
}

int earring_append(Earring *str, const char *suffix, size_t suffix_length) {
    if (str->length + suffix_length > str->max_length) {
        return -1; // Not enough space
    }

    for (size_t i = 0; i < suffix_length; i++) {
        str->data[str->length + i] = suffix[i];
    }

    str->length += suffix_length;

    return 0; // Success
}

int earring_append_char(Earring *str, char c) {
    if (str->length + 1 > str->max_length) {
        return -1; // Not enough space
    }

    str->data[str->length] = c;
    str->length += 1;

    return 0; // Success
}

int earring_string_prepend(Earring *str, const char *prefix, size_t prefix_length) {
    if (str->length + prefix_length > str->max_length) {
        return -1; // Not enough space
    }

    // Shift existing data to the right
    for (size_t i = str->length; i > 0; i--) {
        str->data[i + prefix_length - 1] = str->data[i - 1];
    }

    // Copy the prefix to the beginning
    for (size_t i = 0; i < prefix_length; i++) {
        str->data[i] = prefix[i];
    }

    str->length += prefix_length;

    return 0; // Success
}

int earring_set(Earring *str, const char *new_str, size_t new_str_length) {
    if (new_str_length > str->max_length) {
        return -1; // Not enough space
    }

    for (size_t i = 0; i < new_str_length; i++) {
        str->data[i] = new_str[i];
    }

    str->length = new_str_length;

    return 0; // Success
}

int earring_write(FILE *stream, Earring *er) {
    if (er == NULL) {
        return -1;
    }

    for (size_t i = 0; i < er->length; i++) {
        fputc(er->data[i], stream);
    }

    return 0;
}
