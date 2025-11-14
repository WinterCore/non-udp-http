#include "connection.h"
#include "buffer.h"

HttpConnection create_http_connection(
    Arena *arena,
    int socket_fd,
    in_addr_t client_ip,
    int client_port
) {
    FDBufferReader reader = fd_buffer_reader_create(socket_fd);

    return (HttpConnection) {
        .arena = arena,
        .reader = reader,
        .client_ip = client_ip,
        .client_port = client_port,
    };
}
