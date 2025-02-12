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

printf("Socket created\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]); /* Convert IP address to network byte order */
    server_addr.sin_port = htons(atoi(argv[2])); /* Convert port to network byte order */

    
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
printf("Server binded\n");
/*
Listening for Connections
Prepare to accept connections on socket FD.
N connection requests will be queued before further requests are refused.
Returns 0 on success, -1 for errors.
*/
    if (listen(server_fd, 10) < 0) {
        perror("Listen failed");
        exit(1);
    }

    printf("Server listening on port %s\n", argv[2]);

    if((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
        perror("Accept failed");
        exit(1);
    }


printf("Accepted client\n");
/*
Server waits to receive data from client
Data stored in buffer
bytes_received contains number of bytes received
MAX_BUFFER limits maximum receivable data
*/

    ssize_t bytes_received = read(client_fd, buffer, MAX_BUFFER);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("Received message: %s\n", buffer);
        send(client_fd, buffer, bytes_received, 0);
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
