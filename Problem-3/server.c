#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <asm-generic/socket.h>

#define PORT 8082
#define BUFFER_SIZE 1024

void insertionSort(int32_t* arr, int n) {
    for (int i = 1; i < n; ++i) {
        int32_t key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Sorter Server listening on port %d\n", PORT);

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    uint8_t* recv_buffer = (uint8_t*)malloc(BUFFER_SIZE);
    int bytes_read = read(new_socket, recv_buffer, 2);
    if (bytes_read != 2) {
        fprintf(stderr, "Failed to read element count\n");
        free(recv_buffer);
        close(new_socket);
        close(server_fd);
        return -1;
    }

    uint16_t num_elements = ntohs(*((uint16_t*)recv_buffer));
    int total_received = 2;
    int required_bytes = num_elements * 4;

    while (total_received < required_bytes + 2) {
        bytes_read = read(new_socket, recv_buffer + total_received, BUFFER_SIZE - total_received);
        if (bytes_read <= 0) break;
        total_received += bytes_read;
    }

    int32_t* elements = (int32_t*)malloc(num_elements * sizeof(int32_t));
    for (int i = 0; i < num_elements; ++i) {
        elements[i] = ntohl(*((int32_t*)(recv_buffer + 2 + i*4)));
    }

    insertionSort(elements, num_elements);

    uint8_t* send_buffer = (uint8_t*)malloc(BUFFER_SIZE);
    *((uint16_t*)send_buffer) = htons(num_elements);
    for (int i = 0; i < num_elements; ++i) {
        *((int32_t*)(send_buffer + 2 + i*4)) = htonl(elements[i]);
    }

    int total_sent = 0;
    int bytes_to_send = 2 + num_elements * 4;
    while (total_sent < bytes_to_send) {
        int sent = send(new_socket, send_buffer + total_sent, bytes_to_send - total_sent, 0);
        if (sent <= 0) break;
        total_sent += sent;
    }

    printf("Sent sorted data\n");

    free(elements);
    free(recv_buffer);
    free(send_buffer);
    close(new_socket);
    close(server_fd);
    return 0;
}
