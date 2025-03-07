#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_BUFFER 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int server_fd, client_fd; /*File descriptors for server and client sockets*/
    struct sockaddr_in server_addr, client_addr;
    char buffer[MAX_BUFFER] = {0};
    socklen_t client_len = sizeof(client_addr);

/*
Socket Creation
socket() creates new socket
AF_INET: IPv4 Internet protocols
SOCK_STREAM: TCP connection-based protocol
Returns file descriptor or 0 on error
*/
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(1);
    }



    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1])); /* Convert port to network byte order */

    
/*
Binding Socket
bind() assigns address to socket
Associates socket with specific port
Returns -1 on error
*/
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

/*
Listening for Connections
Prepare to accept connections on socket FD.
N connection requests will be queued before further requests are refused.
Returns 0 on success, -1 for errors.
*/
    if (listen(server_fd, 1) < 0) {
        perror("Listen failed");
        exit(1);
    }

    printf("Server listening on port %s\n", argv[1]);

/*
Accepting Connections
Server blocks/waits for incoming client connection
accept() creates new socket specifically for this client
Returns new file descriptor client_fd for client communication
Stores client's address info in client_addr
*/
    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
        perror("Accept failed");
        exit(1);
    }

/*
Server waits to receive data from client
Data stored in buffer
bytes_received contains number of bytes received
MAX_BUFFER limits maximum receivable data
*/

    ssize_t bytes_received = recv(client_fd, buffer, MAX_BUFFER, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("Received message: %s\n", buffer);
        send(client_fd, buffer, bytes_received, 0);
    }

    close(client_fd);
    close(server_fd);
    return 0;
}