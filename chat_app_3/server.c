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
* Server.c
* Author: Uttam Mahata
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define PORT 8080

void *send_message(void *arg) {
    int new_socket = *((int *)arg);
    char buffer[1024];
    while (1) {
	bzero(buffer, sizeof(buffer));
	printf("Server: ");
	fgets(buffer, 1024, stdin);
	send(new_socket, buffer, strlen(buffer), 0);
    }
}

void *receive_message(void *arg) {
    int new_socket = *((int *)arg);
    char buffer[1024];
    while (1) {
	bzero(buffer, sizeof(buffer));
	recv(new_socket, buffer, 1024, 0);
	printf("Client: %s", buffer);
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
	perror("socket failed");
	exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
	perror("bind failed");
	exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
	perror("listen");
	exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
	perror("accept");
	exit(EXIT_FAILURE);
    }

    pthread_t send_thread, receive_thread;
    pthread_create(&send_thread, NULL, send_message, (void *)&new_socket);
    pthread_create(&receive_thread, NULL, receive_message, (void *)&new_socket);

    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);

    close(new_socket);
    close(server_fd);
    return 0;
}



