#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 8080

void *receive_messages(void *socket_desc) {
    int new_socket = *(int*)socket_desc;
    char buffer[1024];
    
    while (1) {
        int bytes_read = read(new_socket, buffer, sizeof(buffer)-1);
        if (bytes_read <= 0) {
            break;
        }
        buffer[bytes_read] = '\0';
        printf("\nClient: %s\n", buffer);

        // If client sends "Bye", break the loop and terminate
        if (strncmp(buffer, "Bye", 3) == 0) {
            printf("Client ended the chat.\n");
            break;
        }
    }
    
    close(new_socket);
    return NULL;
}

void *send_messages(void *socket_desc) {
    int new_socket = *(int*)socket_desc;
    char buffer[1024];
    
    while (1) {
        printf("Server: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;  
        send(new_socket, buffer, strlen(buffer), 0);
        
        // If server sends "Bye", break the loop and terminate
        if (strncmp(buffer, "Bye", 3) == 0) {
            printf("Server ended the chat.\n");
            break;
        }
    }
    
    close(new_socket);
    return NULL;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    pthread_t receive_thread, send_thread;

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is waiting for client to connect...\n");

    // Accept connection from client
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    printf("Client connected.\n");

    pthread_create(&receive_thread, NULL, receive_messages, (void *)&new_socket);
    pthread_create(&send_thread, NULL, send_messages, (void *)&new_socket);

    // Wait for both threads to finish
    pthread_join(receive_thread, NULL);
    pthread_join(send_thread, NULL);

    // Close server socket
    close(server_fd);
    return 0;
}