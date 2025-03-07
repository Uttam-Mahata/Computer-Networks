#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <asm-generic/socket.h>

#define PORT 8888
#define BUFFER_SIZE 1024

void handle_client_request(int client_socket, int query_type) {
    char response[BUFFER_SIZE] = {0};
    time_t current_time;
    struct tm *time_info;
    
    time(&current_time);
    time_info = localtime(&current_time);
    
    switch(query_type) {
        case 1: // Time only
            strftime(response, BUFFER_SIZE, "%H:%M:%S", time_info);
            printf("Sending time: %s\n", response);
            break;
            
        case 2: // Date and time
            strftime(response, BUFFER_SIZE, "%Y-%m-%d %H:%M:%S", time_info);
            printf("Sending date and time: %s\n", response);
            break;
            
        case 3: // Server name
            if (gethostname(response, BUFFER_SIZE) != 0) {
                strncpy(response, "Unknown host", BUFFER_SIZE);
            }
            printf("Sending server name: %s\n", response);
            break;
            
        default:
            snprintf(response, BUFFER_SIZE, "Error: Unsupported query type %d. Supported types: 1 (time), 2 (date+time), 3 (server name)", 
                    query_type);
            printf("Error: Unsupported query type %d\n", query_type);
            break;
    }
    
    send(client_socket, response, strlen(response), 0);
}

int main() {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    
    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
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
    
    printf("Server is listening on port %d...\n", PORT);
    
    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        
        printf("New client connected\n");
        
        // Read query type from client
        valread = read(new_socket, buffer, BUFFER_SIZE);
        if (valread > 0) {
            int query_type = atoi(buffer);
            printf("Received query type: %d\n", query_type);
            
            handle_client_request(new_socket, query_type);
        }
        
        close(new_socket);
        printf("Client disconnected\n");
    }
    
    return 0;
}