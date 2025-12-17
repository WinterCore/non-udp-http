#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fcntl.h>
#include <arpa/inet.h>
#include <string.h>

#include "connection.h"
#include "aids.h"
#include "arena.h"
#include "request.h"
#include "earring.h"

#define PORT 6969
#define BACKLOG_SIZE 10

int main() {
    int fd = open("./http_request.bin", O_RDONLY);

    if (!fd) {
        perror("open");
        return 1;
    }

    HttpConnection connection = {
        .arena = arena_create(),
        .client_ip = 0,
        .client_port = 0,
        .reader = fd_buffer_reader_create(fd),
    };

    HttpRequest out_request = create_http_request(connection.arena);

    HttpRequestParseResult parse_result = parse_http_request(&connection, &out_request);

    if (parse_result != HTTP_PARSE_OK) {
        PANIC("Failed to parse HTTP request, error code: %d", parse_result);
    }
    
    DEBUG_PRINT("Parsed HTTP Request:\n");
    DEBUG_PRINT("Method: %s\n", out_request.method);
    DEBUG_PRINT("Request target: %s\n", out_request.request_target);
    DEBUG_PRINT("Pathname: %s\n", out_request.pathname);
    DEBUG_PRINT("Query string: %s\n", out_request.query_string);
    DEBUG_PRINT("Version: %s\n", out_request.http_version);
    

    DEBUG_PRINT("\n\n");
    DEBUG_PRINT("Query Params: \n");
    // Print query params
    for (size_t i = 0; i < out_request.query_params.len; i++) {
        DEBUG_PRINT("\t");
        earring_write(stderr, out_request.query_params.keys[i]);
        DEBUG_PRINT(" = ");
        earring_write(stderr, out_request.query_params.values[i]);
        DEBUG_PRINT("\n");
    }

    DEBUG_PRINT("\n\n");
    DEBUG_PRINT("Headers: \n");
    for (size_t i = 0; i < out_request.headers.len; i++) {
        DEBUG_PRINT("\t%s: %s\n", out_request.headers.keys[i], out_request.headers.values[i]);
    }

    DEBUG_PRINT("\n\n");
    
    PANIC("TESTING");

    /*
    FDBufferReader reader = fd_buffer_reader_create(fd);
    
    uint8_t buff[100000] = {0};

    int n = fd_buffer_reader_read_until(&reader, buff, "fhajklsdfhlk", sizeof(buff));

    DEBUG_PRINT("Read %d bytes:\n", n);
    DEBUG_PRINT("%.*s\n", n, buff);
    PANIC("TESTING");
    */

    int server_fd;
    struct sockaddr_in server_addr;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
    }

    // bind socket to port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");

        exit(EXIT_FAILURE);
    }

    // listen for connections
    if (listen(server_fd, BACKLOG_SIZE) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    DEBUG_PRINT("Server listening on port %d\n", PORT);

    uint8_t buffer[80000];

    while (1) {
        Arena *arena = arena_create();

        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = -1;

        if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len)) < 0) {
            perror("accept failed");
            continue;
        }

        DEBUG_PRINT("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        HttpConnection connection = create_http_connection(
            arena,
            client_fd,
            client_addr.sin_addr.s_addr,
            client_addr.sin_port
        );


        HttpRequest out_request = create_http_request(arena);
        
        // Read HTTP request (not fully implemented)
        HttpRequestParseResult request = parse_http_request(&connection, &out_request);

        const char *welcome_msg = "Hello from server!\n";
        
        int bytes_written = sprintf((char *) buffer, "HTTP/1.1 200 OK\r\nContent-Length: %lu\r\n\r\n%s", strlen(welcome_msg), welcome_msg);

        if (send(client_fd, buffer, bytes_written, 0) < 0) {
            perror("send failed");
            
            arena_destroy(arena);
            continue;
        }

        arena_destroy(arena);
    }

    close(server_fd);

    return 0;
}
