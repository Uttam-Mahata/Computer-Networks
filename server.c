#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <asm-generic/signal-defs.h>

#define PORT 8080
#define BACKLOG 10 // how many pending connections queue will hold
#define BUFFER_SIZE 1024

void sigchld_handler(int s)
{
 // waitpid() might overwrite errno, so we save and restore it:
 int saved_errno = errno;

 while(waitpid(-1, NULL, WNOHANG) > 0);
 errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
 if (sa->sa_family == AF_INET) {
 return &(((struct sockaddr_in*)sa)->sin_addr);
 }
 return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
 int server_fd, new_socket;
 struct addrinfo hints, *servinfo, *p;
 struct sockaddr_in address;
 int opt = 1;
 int addrlen = sizeof(address);
 char buffer[BUFFER_SIZE] = {0};
 char response[BUFFER_SIZE] = {0};
 struct sigaction sa;
 int yes=1;
 int rv;

 // Create socket
 if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
 perror("socket failed");
 exit(EXIT_FAILURE);
 }

 // Set socket options
 if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
 perror("setsockopt");
 exit(EXIT_FAILURE);
 }

 address.sin_family = AF_INET;
 address.sin_addr.s_addr = INADDR_ANY;
 address.sin_port = htons(PORT);

 // Bind
 if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
 perror("bind failed");
 exit(EXIT_FAILURE);
 }

 // Listen
 if (listen(server_fd, BACKLOG) < 0) {
 perror("listen");
 exit(EXIT_FAILURE);
 }

 sa.sa_handler = sigchld_handler; // reap all dead processes
 sigemptyset(&sa.sa_mask);
 sa.sa_flags = SA_RESTART;
 if (sigaction(SIGCHLD, &sa, NULL) == -1) {
 perror("sigaction");
 exit(EXIT_FAILURE);
 }

 printf("Server is listening on port %d...\n", PORT);

 // Accept connection
 if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
 perror("accept");
 exit(EXIT_FAILURE);
 }

 // Chat loop
 while(1) {
 memset(buffer, 0, BUFFER_SIZE);
 memset(response, 0, BUFFER_SIZE);

 // Receive message
 int bytes_received = recv(new_socket, buffer, BUFFER_SIZE, 0);
 if (bytes_received <= 0) {
 printf("Client disconnected\n");
 break;
 }

 printf("Client: %s", buffer);

 // Check for "Bye" message
 if (strncmp(buffer, "Bye", 3) == 0) {
 printf("Chat session closed\n");
 break;
 }

 // Get server's response
 printf("Enter response: ");
 fgets(response, BUFFER_SIZE, stdin);

 // Send response
 send(new_socket, response, strlen(response), 0);

 // Check if server wants to end chat
 if (strncmp(response, "Bye", 3) == 0) {
 printf("Chat session closed\n");
 break;
 }
 }

 close(new_socket);
 close(server_fd);
 return 0;
}