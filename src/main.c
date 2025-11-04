#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 6969
#define BACKLOG_SIZE 10

int main() {
    int server_fd;
    struct sockaddr_in server_addr;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

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

    printf("Server listening on port %d\n", PORT);

    char buffer[1024];
    
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = -1;

        if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len)) < 0) {
            perror("accept failed");
            continue;
        }

        printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        const char *welcome_msg = "Hello from server!\n";
        
        int bytes_written = sprintf(buffer, "HTTP/1.1 200 OK\r\nContent-Length: %lu\r\n\r\n%s", strlen(welcome_msg), welcome_msg);

        if (send(client_fd, buffer, bytes_written, 0) < 0) {
            perror("send failed");
            continue;
        }
    }

    close(server_fd);

    return 0;
}
