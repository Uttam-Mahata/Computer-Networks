/*
 * Basic Chat Application
 * Develop a simple TCP Client-Server application where The client app sends the first
message it wishes to. Upon receiving that message, the server app prints the received
message. Then, it sends the response message to the client. This message exchange continues
until one party (server or client) says "Bye" and closes the chat session. After receiving the
"Bye" message, another party closes the chat session.
Note that in this way of chatting, one end cannot send more than one message at a time. On
sending one message, it has to wait for the response from another party, and then only it can
send the next message.
* Author: Uttam Mahata
* Client.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080

int main()
{
    int client_socket;
    struct sockaddr_in server_address;
    char buffer[1024] = {0};
    char message[1024] = {0};

    // Create a socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
	perror("Socket creation failed");
	exit(EXIT_FAILURE);
    }

    // Initialize the server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "192.168.29.22", &server_address.sin_addr) <= 0)
    {
	perror("Invalid address/ Address not supported");
	exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
	perror("Connection failed");
	exit(EXIT_FAILURE);
    }

    // Chat
    while (1)
    {
	printf("Enter a message: ");
	fgets(message, 1024, stdin);
	send(client_socket, message, strlen(message), 0);
	printf("Message sent\n");

	recv(client_socket, buffer, 1024, 0);
	printf("Server: %s\n", buffer);

	if (strncmp(buffer, "Bye", 3) == 0)
	{
	    printf("Chat session closed\n");
	    break;
	}
    }

    close(client_socket);
    return 0;
}


