#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <inttypes.h>

#include "./helpers.h"

int main() {

    struct sockaddr_in server_sockaddr_in;

    server_sockaddr_in.sin_family = AF_INET;
    server_sockaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);

    const int port = 6969;

    server_sockaddr_in.sin_port = htons(port);

    int fd = socket(AF_INET, SOCK_STREAM, 0);

    int optval = 1;

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) != 0) {
        PANIC("Failed to set socket opt");
    }

    int bind_result = bind(fd, (struct sockaddr *) &server_sockaddr_in, sizeof(server_sockaddr_in));

    if (bind_result != 0) {
        PANIC("Failed to bind");
    }

    int listen_result = listen(fd, 10);

    if (listen_result != 0) {
        PANIC("Failed to listen");
    }

    DEBUG_PRINT("SERVER is up and running...\n");

    while (true) {
        struct sockaddr_in client_sockaddr_in;
        socklen_t len = sizeof(client_sockaddr_in);

        int client_fd = accept(
            fd,
            (struct sockaddr *) &client_sockaddr_in,
            &len
        );

        DEBUG_PRINTF("CLIENT CONNECTED!\n\tIP: %s:%" PRIu16, inet_ntoa(client_sockaddr_in.sin_addr), client_sockaddr_in.sin_port);

        uint8_t buffer[4000] = {0};


        ssize_t bytes_read;

        do {
            bytes_read = read(client_fd, buffer, sizeof(buffer));
            buffer[bytes_read] = '\0';

            DEBUG_PRINTF("DATA RECEIVED(%zd): %s", bytes_read, buffer);
        } while (bytes_read > 0);

        if (bytes_read == 0) {
            DEBUG_PRINT("CLIENT DISCONNECTED");
        }

        if (bytes_read == -1) {
            perror("Failed to read from client");
        }
    }

    return 0;
}
