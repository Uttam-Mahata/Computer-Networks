#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080

void *receive_messages(void *socket_desc) {
    int client_socket = *(int*)socket_desc;
    char buffer[1024];
    
    while (1) {
        int bytes_read = read(client_socket, buffer, sizeof(buffer)-1);
        if (bytes_read <= 0) {
            break; 
        }
        buffer[bytes_read] = '\0';
        printf("\nServer: %s\n", buffer);
        
        // If server sends "Bye", break the loop and terminate
        if (strncmp(buffer, "Bye", 3) == 0) {
            printf("Server ended the chat.\n");
            break;
        }
    }

    close(client_socket);
    return NULL;
}

void *send_messages(void *socket_desc) {
    int client_socket = *(int*)socket_desc;
    char buffer[1024];
    
    while (1) {
        printf("Client: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        
        send(client_socket, buffer, strlen(buffer), 0);
        
        // If client sends "Bye", break the loop and terminate
        if (strncmp(buffer, "Bye", 3) == 0) {
            printf("Client ended the chat.\n");
            break;
        }
    }
    
    close(client_socket);
    return NULL;
}

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    pthread_t receive_thread, send_thread;

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    // Convert IPv4 address to binary form
    if (inet_pton(AF_INET, "10.2.81.238", &server_address.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    printf("Connected to server.\n");

    pthread_create(&receive_thread, NULL, receive_messages, (void *)&client_socket);
    pthread_create(&send_thread, NULL, send_messages, (void *)&client_socket);

    // Wait for both threads to finish
    pthread_join(receive_thread, NULL);
    pthread_join(send_thread, NULL);

    // Close client socket
    close(client_socket);
    return 0;
}