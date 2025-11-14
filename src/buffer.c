#include "buffer.h"
#include "aids.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CAPACITY 4096

FDBufferReader fd_buffer_reader_create(int fd) {
    uint8_t *buffer = malloc(sizeof(uint8_t) * CAPACITY);

    FDBufferReader reader = {
        .fd = fd,
        .eof_reached = false,
        .buffer = buffer,
        .capacity = CAPACITY,
        .length = 0,
        .position = 0,
    };

    return reader;
}


int fd_buffer_reader_fill(FDBufferReader *reader) {
    if (reader->position >= reader->length) {
        int n = read(reader->fd, reader->buffer, reader->capacity);

        if (n < 0) {
            return -1;
        }

        if (n == 0) {
            reader->eof_reached = true;
            
            return 0;
        }

        reader->length = n;
        reader->position = 0;
    }

    return reader->length - reader->position;
}

BufferReaderReadResult fd_buffer_reader_read_until(
    FDBufferReader *reader,
    uint8_t *dest,
    char *delimiter,
    int max_bytes,
    int *bytes_read
) {
    *bytes_read = 0;

    if (reader->eof_reached) {
        return -3;
    }

    int delimiter_pos = 0;
    int delimiter_length = strlen(delimiter);
    
    while (max_bytes > 0) {
        // Fill buffer if empty
        int bytes_remaining = fd_buffer_reader_fill(reader);

        if (bytes_remaining < 0) {
            // Error reading from fd
            return BUFFER_READER_READ_FD_ERROR;
        }

        if (bytes_remaining == 0) {
            // EOF reached
            return BUFFER_READER_READ_EOF_BEFORE_DELIMITER;
        }

        // Copy until buffer is exhausted or delimiter is found
        for (; reader->position < reader->length && max_bytes > 0; reader->position += 1) {

            dest[*bytes_read] = reader->buffer[reader->position];

            *bytes_read += 1;
            max_bytes -= 1;

            if (dest[*bytes_read - 1] == delimiter[delimiter_pos]) {
                delimiter_pos += 1;

                // Delimiter found
                if (delimiter_pos == delimiter_length) {
                    reader->position += 1;

                    *bytes_read -= delimiter_length;

                    return BUFFER_READER_READ_OK;
                }
            } else {
                delimiter_pos = 0;
            }
        }
    }

    return BUFFER_READER_READ_MAX_BYTES_EXCEEDED;
}

int fd_buffer_reader_read(FDBufferReader *reader, uint8_t *dest, int bytes_to_read) {
    int bytes_read = 0;

    if (reader->eof_reached) {
        return 0;
    }

    while (bytes_to_read > 0) {
        // Fill buffer if empty
        int bytes_remaining = fd_buffer_reader_fill(reader);

        if (bytes_remaining < 0) {
            // Error reading from fd
            return -1;
        }

        if (bytes_remaining == 0) {
            // EOF reached
            break;
        }

        int bytes_available = reader->length - reader->position;
        int bytes_to_copy = MIN(bytes_available, bytes_to_read);

        memcpy(dest, reader->buffer + reader->position, bytes_to_copy);

        reader->position += bytes_to_copy;
        bytes_read += bytes_to_copy;
        bytes_to_read -= bytes_to_copy;
    }

    return bytes_read;
}


void fd_buffer_reader_destroy(FDBufferReader *reader) {
    free(reader->buffer);
}
