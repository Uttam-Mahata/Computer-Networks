/*
 * Develop a simple TCP Client-Server application where the client app sends a predefined text
message "Hello World!!" to the server app running in a user-defined port of your choice.
Upon receiving that message, the server app forwards the same message to the client app.
Both server and client applications print the message.
* Author: Uttam Mahata
* Client.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char buffer[1024] = {0};
    char *hello = "Hello World!!";

    // Create a socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	perror("Socket creation failed");
	exit(EXIT_FAILURE);
    }

    // Initialize the server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "192.168.29.22", &server_address.sin_addr) <= 0) {
	perror("Invalid address/ Address not supported");
	exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
	perror("Connection failed");
	exit(EXIT_FAILURE);
    }

    // Send the message to the server
    send(client_socket, hello, strlen(hello), 0);
    printf("Message sent to the server\n");

    // Receive the message from the server
    read(client_socket, buffer, 1024);
    printf("Message from the server: %s\n", buffer);

    return 0;
}


