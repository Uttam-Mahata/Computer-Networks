#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_BUFFER 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUFFER] = {0};
    char *message = "Hello World!!";

    // Create socket
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));

    // Convert IP address
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(1);
    }

    // Connect to server
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    // Send message
    printf("Sending message: %s\n", message);
    send(sock_fd, message, strlen(message), 0);

    // Receive echo
    ssize_t bytes_received = recv(sock_fd, buffer, MAX_BUFFER, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("Server echo: %s\n", buffer);
    }

    close(sock_fd);
    return 0;
}