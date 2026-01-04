#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fcntl.h>
#include <arpa/inet.h>
#include <string.h>

#include "buffer.h"
#include "connection.h"
#include "aids.h"
#include "arena.h"
#include "request.h"
#include "earring.h"
#include "response.h"

#define PORT 6969
#define BACKLOG_SIZE 10

int main() {
    /*
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
    */
    
    // Test response serializer

    /*
    char BUFFER[1024] = {0};
    Arena *arena = arena_create();

    HttpResponse response = create_http_response(arena);
    http_response_add_header(&response, "Accept", "application/json");
    http_response_add_header(&response, "Content-Type", "application/json");
    http_response_add_header(&response, "Potato", "is hot");

    BufferWriter writer = buffer_writer_create(BUFFER, sizeof(BUFFER));
    int bytes_written = http_response_serialize_head(&response, &writer);
    BUFFER[bytes_written] = '\0';

    DEBUG_PRINT("%s", BUFFER);

    return 0;
    */

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

    char buffer[80000];

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

        const char *welcome_json = "{ \"message\": \"Don't come here again!\" }";
            
        HttpResponse response = create_http_response(arena);
        http_response_add_header(&response, "Accept", "application/json");
        http_response_add_header(&response, "Content-Type", "application/json");
        size_t body_len = strlen(welcome_json);
        char content_length[12];
        snprintf(content_length, sizeof(content_length), "%zu", body_len);
        http_response_add_header(&response, "Content-Length", content_length);
        
        BufferWriter writer = buffer_writer_create(buffer, sizeof(buffer));
        int response_bytes = http_response_serialize_head(&response, &writer);
        response_bytes += buffer_write_str(&writer, welcome_json);

        if (send(client_fd, buffer, response_bytes, 0) < 0) {
            perror("send failed");
            
            arena_destroy(arena);
            continue;
        }

        close(client_fd);

        arena_destroy(arena);
    }

    close(server_fd);

    return 0;
}
