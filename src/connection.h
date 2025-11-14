#ifndef CONNECTION_H
#define CONNECTION_H
#include "arena.h"
#include "buffer.h"
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

#endif
