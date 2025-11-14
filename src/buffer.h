#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct FDBufferReader {
    int fd;
    bool eof_reached;

    uint8_t *buffer;
    int capacity;

    int length;
    int position;
} FDBufferReader;

FDBufferReader fd_buffer_reader_create(int fd);
int fd_buffer_reader_read(FDBufferReader *reader, uint8_t *dest, int bytes_to_read);
int fd_buffer_reader_read_until(FDBufferReader *reader, uint8_t *dest, char *delimiter, int max_bytes);
void fd_buffer_reader_destroy(FDBufferReader *reader);

#endif
