#ifndef CONNECTION_H
#define CONNECTION_H
#include "arena.h"
#include "buffer.h"
#include "response.h"
#include <netinet/in.h>

typedef struct HttpConnection {
    Arena *arena;

    FDBufferReader reader;
    
    in_addr_t client_ip;
    int client_port;
} HttpConnection;

HttpConnection create_http_connection(
    Arena *arena,
    int socket_fd,
    in_addr_t client_ip,
    int client_port
);

// Send status line and headers to the client
// Returns 0 on success, -1 on error
int connection_begin_response(HttpConnection *conn, HttpResponse *response);

// Write body data to the client (can be called multiple times for streaming)
// Returns 0 on success, -1 on error
int connection_write_body(HttpConnection *conn, const char *data, size_t len);

// Finalize the response
// Returns 0 on success, -1 on error
int connection_end_response(HttpConnection *conn);

#endif
