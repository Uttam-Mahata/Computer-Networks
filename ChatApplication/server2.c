#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_BUFFER 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[MAX_BUFFER] = {0};
    char response[MAX_BUFFER];
    socklen_t client_len = sizeof(client_addr);

    // Create and setup socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    if (listen(server_fd, 1) < 0) {
        perror("Listen failed");
        exit(1);
    }

    printf("Server listening on port %s\n", argv[1]);

    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
        perror("Accept failed");
        exit(1);
    }

    printf("Client connected. Starting chat...\n");

    while (1) {
        // Receive client message
        memset(buffer, 0, MAX_BUFFER);
        int bytes_received = recv(client_fd, buffer, MAX_BUFFER, 0);
        if (bytes_received <= 0) break;
        
        printf("Client: %s\n", buffer);
        
        if (strcmp(buffer, "Bye") == 0) {
            printf("Client ended the chat\n");
            break;
        }

        // Get server's response
        printf("Server: ");
        fgets(response, MAX_BUFFER, stdin);
        response[strcspn(response, "\n")] = 0;

        // Send response
        send(client_fd, response, strlen(response), 0);
        
        if (strcmp(response, "Bye") == 0) {
            printf("Ending chat\n");
            break;
        }
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
