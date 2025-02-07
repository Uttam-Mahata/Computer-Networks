#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void insertion_sort(int arr[], int n) {
    int i, key, j;
    for (i = 1; i < n; i++) {
        key = arr[i];
        j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
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

    printf("Server listening on port %s\n", argv[1]);

    while(1) {
        printf("Waiting for client connection...\n");
        if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
            perror("Accept failed");
            continue;
        }
        printf("Client connected from %s:%d\n", 
               inet_ntoa(client_addr.sin_addr), 
               ntohs(client_addr.sin_port));

        uint16_t num_elements;
        ssize_t bytes_received = recv(client_fd, &num_elements, sizeof(uint16_t), 0);
        if (bytes_received != sizeof(uint16_t)) {
            perror("Error receiving number of elements");
            close(client_fd);
            continue;
        }
        num_elements = ntohs(num_elements);
        printf("Expecting %d numbers\n", num_elements);

        int *numbers = malloc(num_elements * sizeof(int));
        if (!numbers) {
            perror("Memory allocation failed");
            close(client_fd);
            continue;
        }

        // Receive all numbers in one go
        ssize_t expected_bytes = num_elements * sizeof(uint32_t);
        bytes_received = recv(client_fd, numbers, expected_bytes, MSG_WAITALL);
        if (bytes_received != expected_bytes) {
            perror("Error receiving numbers");
            free(numbers);
            close(client_fd);
            continue;
        }

        // Convert network byte order to host byte order
        for(int i = 0; i < num_elements; i++) {
            numbers[i] = ntohl(((uint32_t*)numbers)[i]);
            printf("Received number %d: %d\n", i+1, numbers[i]);
        }

        printf("Sorting %d numbers...\n", num_elements);
        insertion_sort(numbers, num_elements);
        
        printf("Sending sorted numbers back...\n");
        // Convert to network byte order
        for(int i = 0; i < num_elements; i++) {
            ((uint32_t*)numbers)[i] = htonl(numbers[i]);
        }

        if (send(client_fd, numbers, num_elements * sizeof(uint32_t), 0) != num_elements * sizeof(uint32_t)) {
            perror("Error sending sorted numbers");
        }

        free(numbers);
        close(client_fd);
        printf("Client disconnected\n");
        exit(0);
        
    }

    close(server_fd);
    return 0;
}
