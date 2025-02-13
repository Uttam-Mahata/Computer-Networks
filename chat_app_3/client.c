/*
 * Develop a simple TCP Client-Server application where The client app sends message it wishes
 * to the server.The chatting mechanism
should be flexible, so sending and receiving messages should be concurrent. In other words,
sending and receiving messages should not block each other. Users may send no or multiple
messages before receiving a response from another party.
It can be achieved using the Process or Thread concept, where "reading a text from a user and
sending" job can be given to a Process or a Thread. Again, the "receiving a text message and
printing it in the console" job can be given to another Process or a Thread. In this assignment,
you are encouraged to use the Thread concept instead of Process.
You are suggested to make sure that the termination of the chat session happens gracefully!!
* 
* Client.c
* Author: Uttam Mahata
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080

void *send_message(void *arg) {
    int client_socket = *((int *)arg);
    char buffer[1024];
    while (1) {
	bzero(buffer, sizeof(buffer));
	printf("Client: ");
	fgets(buffer, 1024, stdin);
	send(client_socket, buffer, strlen(buffer), 0);
    }
}

void *receive_message(void *arg) {
    int client_socket = *((int *)arg);
    char buffer[1024];
    while (1) {
	bzero(buffer, sizeof(buffer));
	recv(client_socket, buffer, 1024, 0);
	printf("Server: %s", buffer);
    }
}

int main() {
    int client_socket;
    struct sockaddr_in server_address;

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
	perror("socket failed");
	exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "192.168.29.22", &server_address.sin_addr) <= 0) {
	perror("inet_pton failed");
	exit(EXIT_FAILURE);
    }

    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
	perror("connect failed");
	exit(EXIT_FAILURE);
    }

    pthread_t send_thread, receive_thread;
    pthread_create(&send_thread, NULL, send_message, (void *)&client_socket);
    pthread_create(&receive_thread, NULL, receive_message, (void *)&client_socket);

    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);

    close(client_socket);
    return 0;
}
