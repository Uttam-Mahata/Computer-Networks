#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <asm-generic/socket.h>

#define PORT 8888
#define MAX_ELEMENTS 1000
#define MAX_PACKET_SIZE (2 + (MAX_ELEMENTS * 4))  // 2 bytes for count + 4 bytes per element

void insertionSort(int arr[], int n) {
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

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char packet_buffer[MAX_PACKET_SIZE];
    char response_packet[MAX_PACKET_SIZE];

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

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        printf("\nNew client connected\n");

        // Read the entire packet
        int bytes_read = 0;
        uint16_t num_elements;
        
        // First read 2 bytes to get the number of elements
        bytes_read = recv(new_socket, packet_buffer, 2, 0);
        if (bytes_read != 2) {
            perror("Failed to read packet size");
            close(new_socket);
            continue;
        }

        // Extract number of elements
        memcpy(&num_elements, packet_buffer, 2);
        num_elements = ntohs(num_elements);
        printf("Number of elements: %d\n", num_elements);

        if (num_elements > MAX_ELEMENTS) {
            printf("Too many elements requested\n");
            close(new_socket);
            continue;
        }

        // Read the elements
        int expected_bytes = num_elements * 4;
        bytes_read = recv(new_socket, packet_buffer + 2, expected_bytes, MSG_WAITALL);
        if (bytes_read != expected_bytes) {
            perror("Failed to read elements");
            close(new_socket);
            continue;
        }

        // Extract elements
        int elements[MAX_ELEMENTS];
        for (int i = 0; i < num_elements; i++) {
            int32_t network_value;
            memcpy(&network_value, packet_buffer + 2 + (i * 4), 4);
            elements[i] = ntohl(network_value);
            printf("Element %d: %d\n", i + 1, elements[i]);
        }

        // Sort the elements
        insertionSort(elements, num_elements);
        printf("Sorted elements: ");
        for (int i = 0; i < num_elements; i++) {
            printf("%d ", elements[i]);
        }
        printf("\n");

        // Prepare response packet
        uint16_t network_num = htons(num_elements);
        memcpy(response_packet, &network_num, 2);
        
        for (int i = 0; i < num_elements; i++) {
            int32_t network_value = htonl(elements[i]);
            memcpy(response_packet + 2 + (i * 4), &network_value, 4);
        }

        // Send response packet
        int total_size = 2 + (num_elements * 4);
        send(new_socket, response_packet, total_size, 0);

        close(new_socket);
        printf("Client disconnected\n");
    }

    return 0;
}
