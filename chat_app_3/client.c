#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Global variables for clean shutdown
volatile int running = 1;
int client_socket = -1;

// Thread function to receive messages
void* receive_messages(void* socket_ptr) {
    int sock = *(int*)socket_ptr;
    char buffer[BUFFER_SIZE];
    
    while(running) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(sock, buffer, BUFFER_SIZE, 0);
        
        if (bytes_received <= 0) {
            printf("\nServer disconnected\n");
            running = 0;
            break;
        }
        
        if (strcmp(buffer, "Bye") == 0) {
            printf("\nServer sent Bye. Closing connection...\n");
            running = 0;
            break;
        }
        
        printf("\nServer: %s\n", buffer);
    }
    return NULL;
}

// Thread function to send messages
void* send_messages(void* socket_ptr) {
    int sock = *(int*)socket_ptr;
    char message[BUFFER_SIZE];
    
    while(running) {
        memset(message, 0, BUFFER_SIZE);
        if (fgets(message, BUFFER_SIZE, stdin) != NULL) {
            message[strcspn(message, "\n")] = 0;  // Remove newline
            
            send(sock, message, strlen(message), 0);
            
            if (strcmp(message, "Bye") == 0) {
                printf("Sending Bye to server. Closing connection...\n");
                running = 0;
                break;
            }
        }
    }
    return NULL;
}

// Signal handler for graceful shutdown
void handle_signal(int sig) {
    running = 0;
    if (client_socket != -1) {
        send(client_socket, "Bye", strlen("Bye"), 0);
        close(client_socket);
    }
    printf("\nClient shutting down...\n");
    exit(0);
}

int main() {
    struct sockaddr_in server_address;
    pthread_t recv_thread, send_thread;
    
    // Set up signal handler
    signal(SIGINT, handle_signal);
    
    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    
    // Convert IPv4 address from text to binary
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }
    
    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Connected to server. Start chatting (type 'Bye' to exit)!\n");
    
    // Create threads for sending and receiving
    pthread_create(&recv_thread, NULL, receive_messages, &client_socket);
    pthread_create(&send_thread, NULL, send_messages, &client_socket);
    
    // Wait for threads to complete
    pthread_join(recv_thread, NULL);
    pthread_join(send_thread, NULL);
    
    // Clean up
    close(client_socket);
    printf("Client terminated.\n");
    return 0;
}