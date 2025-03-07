#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#define MAX_BUFFER 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[MAX_BUFFER] = {0};
    char time_str[MAX_BUFFER] = {0};
    socklen_t client_len = sizeof(client_addr);

    // Create socket
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

    printf("Time Server listening on port %s\n", argv[1]);

    while(1) {
        if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
            perror("Accept failed");
            continue;
        }

        // Receive client request
        ssize_t bytes_received = recv(client_fd, buffer, MAX_BUFFER, 0);
        if (bytes_received > 0) {
            // Get current time
            time_t now = time(NULL);
            strftime(time_str, MAX_BUFFER, "%Y-%m-%d %H:%M:%S", localtime(&now));
            
            // Send time to client
            send(client_fd, time_str, strlen(time_str), 0);
            printf("Time sent to client: %s\n", time_str);
        }

        close(client_fd);
        exit(0);
    }

    close(server_fd);
    return 0;
}