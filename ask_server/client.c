#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8888
#define BUFFER_SIZE 1024

void print_menu() {
    printf("\nAvailable queries:\n");
    printf("1: Get server time\n");
    printf("2: Get server date and time\n");
    printf("3: Get server name\n");
    printf("0: Exit\n");
    printf("Enter your choice: ");
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char query[BUFFER_SIZE] = {0};
    int choice;
    
    while(1) {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while(getchar() != '\n'); // Clear input buffer
            continue;
        }
        
        if (choice == 0) {
            printf("Exiting...\n");
            break;
        }
        
        // Create socket
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("Socket creation failed");
            exit(EXIT_FAILURE);
        }
        
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);
        
        // Convert IP address from string to binary
        if (inet_pton(AF_INET, "192.168.29.22", &serv_addr.sin_addr) <= 0) {
            perror("Invalid address");
            exit(EXIT_FAILURE);
        }
        
        // Connect to server
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            perror("Connection failed");
            exit(EXIT_FAILURE);
        }
        
        printf("Connected to server.\n");
        
        // Convert choice to string and send
        snprintf(query, BUFFER_SIZE, "%d", choice);
        send(sock, query, strlen(query), 0);
        printf("Query sent: %d\n", choice);
        
        // Receive server's response
        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(sock, buffer, BUFFER_SIZE);
        
        // Print response
        switch(choice) {
            case 1:
                printf("Server time: %s\n", buffer);
                break;
            case 2:
                printf("Server date and time: %s\n", buffer);
                break;
            case 3:
                printf("Server name: %s\n", buffer);
                break;
            default:
                printf("Server response: %s\n", buffer);
                break;
        }
        
        close(sock);
    }
    
    return 0;
}