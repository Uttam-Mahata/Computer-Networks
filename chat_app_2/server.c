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
* Server.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <asm-generic/socket.h>

#define PORT 8080
#define MAX 1024

int main()
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[MAX] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
	perror("socket failed");
	exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
	perror("setsockopt");
	exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
	perror("bind failed");
	exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
	perror("listen");
	exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
	perror("accept");
	exit(EXIT_FAILURE);
    }

    while (1)
    {
	valread = read(new_socket, buffer, MAX);
	printf("Client: %s\n", buffer);
	if (strcmp(buffer, "Bye") == 0)
	{
	    printf("Server: Bye\n");
	    send(new_socket, "Bye", strlen("Bye"), 0);
	    // break;
        exit(0);
	}
	printf("Server: ");
	fgets(buffer, MAX, stdin);
	send(new_socket, buffer, strlen(buffer), 0);
    }
    return 0;
}
