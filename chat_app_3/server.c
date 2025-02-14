#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <asm-generic/socket.h>

#define PORT 8080
#define MAX 1024

// Global variables for clean shutdown
volatile int running = 1;
int client_socket = -1;

// Thread function to receive messages
void* receive_messages(void* socket_ptr) {
    int sock = *(int*)socket_ptr;
    char buffer[MAX];
    
    while(running) {
        memset(buffer, 0, MAX);
        int valread = read(sock, buffer, MAX);
        
        if (valread <= 0) {
            printf("\nClient disconnected\n");
            running = 0;
            break;
        }
        
        // Remove newline if present
        buffer[strcspn(buffer, "\n")] = 0;
        
        if (strcmp(buffer, "Bye") == 0) {
            printf("\nClient sent Bye. Closing connection...\n");
            running = 0;
            break;
        }
        
        printf("\nClient: %s\n", buffer);
    }
    return NULL;
}

// Thread function to send messages
void* send_messages(void* socket_ptr) {
    int sock = *(int*)socket_ptr;
    char buffer[MAX];
    
    while(running) {
        memset(buffer, 0, MAX);
        if (fgets(buffer, MAX, stdin) != NULL) {
            buffer[strcspn(buffer, "\n")] = 0;  // Remove newline
            
            send(sock, buffer, strlen(buffer), 0);
            
            if (strcmp(buffer, "Bye") == 0) {
                printf("Sending Bye to client. Closing connection...\n");
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
    printf("\nServer shutting down...\n");
    exit(0);
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    pthread_t recv_thread, send_thread;
    
    // Set up signal handler
    signal(SIGINT, handle_signal);
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    printf("Server is listening on port %d...\n", PORT);
    
    // Accept connection
    if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    
    printf("Client connected. Start chatting!\n");
    
    // Create threads for sending and receiving
    pthread_create(&recv_thread, NULL, receive_messages, &client_socket);
    pthread_create(&send_thread, NULL, send_messages, &client_socket);
    
    // Wait for threads to complete
    pthread_join(recv_thread, NULL);
    pthread_join(send_thread, NULL);
    
    // Clean up
    close(client_socket);
    close(server_fd);
    printf("Server terminated.\n");
    return 0;
}