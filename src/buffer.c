#include "buffer.h"
#include <stdio.h>
#include <string.h>


BufferWriter buffer_writer_create(char *buffer, size_t capacity) {
    BufferWriter writer = {0};

    writer.buffer = buffer;
    writer.capacity = capacity;
    writer.position = 0;

    return writer;
}

// Returns bytes written, or -1 if not enough space
int buffer_write(BufferWriter *writer, const char *data, size_t len) {
    if (writer->capacity - writer->position < len) {
        return -1;
    }
    
    strncpy(&writer->buffer[writer->position], data, len);
    writer->position += len;

    return len;
}

int buffer_write_str(BufferWriter *writer, const char *str) {
    size_t len = strlen(str);

    return buffer_write(writer, str, len);
}

int buffer_write_char(BufferWriter *writer, char c) {
    if (writer->capacity - writer->position < 1) {
        return -1;
    }

    writer->buffer[writer->position] = c;
    writer->position += 1;

    return 1;
}

int buffer_write_int_as_string(BufferWriter *writer, int value) {
    char buf[12]; // enough for 32-bit int + sign + null
    int digits_written = snprintf(buf, sizeof(buf), "%d", value);
    
    return buffer_write(writer, buf, digits_written);
}
