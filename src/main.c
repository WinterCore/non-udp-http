#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include "helpers.h"
#include "hector.h"

#define PORT 6969
#define MAX_CONNECTIONS 1

int main() {
    Hector hector = hector_create(sizeof(float));
    hector_push(&hector, (float[]) { 0.5f });
    printf("Capacity after push: %zu\n\n", hector.capacity);
    hector_pop(&hector);
    printf("Capacity after shrink: %zu\n\n", hector.capacity);
    hector_push(&hector, (float[]) { 0.5f });
    printf("Capacity after 2nd push: %zu\n\n", hector.capacity);

    size_t len = hector_len(&hector);
    printf("Len: %zu\n\n", len);
    for (size_t i = 0; i < len; i += 1) {
        printf("Elem(%zu): %f\n", i, *((float *) hector_get(&hector, i)));
    }

    PANIC("END");


    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == -1) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    printf("Socket created successfully!\n");

    
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &(int []) {1}, sizeof(int)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr = {0};

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);
    

    int bind_result = bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));

    if (bind_result == -1) {
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }

    printf("Socket bound successfully!\n");

    int listen_result = listen(server_fd, MAX_CONNECTIONS);

    if (listen_result == -1) {
        perror("Failed to listen!");
        exit(EXIT_FAILURE);
    }

    printf("Server is running port %d!\n\n", PORT);


    while (true) {
        struct sockaddr_in client_addr = {0};
        socklen_t len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &len);

        if (client_fd == -1) {
            perror("Failed to accept client!\n");
            continue;
        }

        printf("Client connected successfully!\n");

        while (true) {
            uint8_t buffer[5000];
            ssize_t bytes_count = recv(client_fd, buffer, sizeof(buffer), 0);

            if (bytes_count == -1) {
                perror("Failed to receive data from client\n");
                continue;
            }

            if (bytes_count == 0) {
                printf("Client has disconnected!\n");
                break;
            }

            buffer[bytes_count] = '\0';

            printf("Received data: %s", buffer);

            int resp_len = sprintf((char *) buffer, "HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nabc");

            int send_result = send(client_fd, buffer, resp_len, 0);

            if (send_result == -1) {
                perror("Failed to send data to client");
                break;
            }
        }
    }

    return 0;
}

