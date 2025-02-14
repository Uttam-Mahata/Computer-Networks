#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <asm-generic/socket.h>

#define PORT 8080
#define MAX 1024

volatile int running = 1;
int client_socket;

void *send_message(void *arg) {
    char buffer[MAX];
    while(running) {
        printf("Server: ");
        fgets(buffer, MAX, stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        
        send(client_socket, buffer, strlen(buffer), 0);
        
        if (strcmp(buffer, "Bye") == 0) {
            running = 0;
            break;
        }
    }
    return NULL;
}

void *receive_message(void *arg) {
    char buffer[MAX];
    int valread;
    
    while(running) {
        memset(buffer, 0, MAX);
        valread = read(client_socket, buffer, MAX);
        
        if (valread <= 0 || strcmp(buffer, "Bye") == 0) {
            running = 0;
            break;
        }
        
        printf("\nClient: %s\n", buffer);
    }
    return NULL;
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    pthread_t send_thread, receive_thread;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
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

    printf("Server is listening on port %d...\n", PORT);

    if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("Client connected. Start chatting!\n");

    pthread_create(&send_thread, NULL, send_message, NULL);
    pthread_create(&receive_thread, NULL, receive_message, NULL);

    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);

    printf("Chat session ended.\n");
    close(client_socket);
    close(server_fd);
    return 0;
}
