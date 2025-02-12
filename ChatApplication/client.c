#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

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

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(argv[1], argv[2], &hints, &res);
    if (status != 0) {
        perror("getaddrinfo failed");
        exit(1);
    }

    // Create socket
    if ((sock_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // server_addr.sin_family = AF_INET;
    // server_addr.sin_port = htons(atoi(argv[1]));

    // Convert IP address
    // if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        
    //     perror("Invalid address");
    //     exit(1);
    // }
    // server_addr.sin_addr.s_addr = inet_addr("10.2.81.238");
    // Connect to server
    if (connect(sock_fd, res->ai_addr, res->ai_addrlen) < 0) {
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
