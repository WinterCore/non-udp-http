#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>

typedef struct BufferWriter {
  char *buffer;
  size_t capacity;
  size_t position;
} BufferWriter;

BufferWriter buffer_writer_create(char *buffer, size_t capacity);

// Returns bytes written, or -1 if not enough space
int buffer_write(BufferWriter *writer, const char *data, size_t len);
int buffer_write_str(BufferWriter *writer, const char *str);
int buffer_write_char(BufferWriter *writer, char c);
int buffer_write_int_as_string(BufferWriter *writer, int value);


#endif
