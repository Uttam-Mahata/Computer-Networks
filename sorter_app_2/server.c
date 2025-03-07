#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <asm-generic/socket.h>

#define PORT 8888
#define MAX_ELEMENTS 1000
#define MAX_PACKET_SIZE (2 + (MAX_ELEMENTS * 4))
#define MAX_CLIENTS 10

typedef struct {
    int client_socket;
    pthread_t thread_id;
} client_data;

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

void *handle_client(void *arg) {
    client_data *data = (client_data *)arg;
    int client_socket = data->client_socket;
    char packet_buffer[MAX_PACKET_SIZE];
    char response_packet[MAX_PACKET_SIZE];
    uint16_t num_elements;

    // Read packet size
    int bytes_read = recv(client_socket, packet_buffer, 2, 0);
    if (bytes_read != 2) {
        perror("Failed to read packet size");
        close(client_socket);
        free(data);
        pthread_exit(NULL);
    }

    memcpy(&num_elements, packet_buffer, 2);
    num_elements = ntohs(num_elements);
    printf("[Thread %lu] Processing %d elements\n", pthread_self(), num_elements);

    if (num_elements > MAX_ELEMENTS) {
        printf("[Thread %lu] Too many elements requested\n", pthread_self());
        close(client_socket);
        free(data);
        pthread_exit(NULL);
    }

    // Read elements
    int expected_bytes = num_elements * 4;
    bytes_read = recv(client_socket, packet_buffer + 2, expected_bytes, MSG_WAITALL);
    if (bytes_read != expected_bytes) {
        perror("Failed to read elements");
        close(client_socket);
        free(data);
        pthread_exit(NULL);
    }

    // Extract and sort elements
    int elements[MAX_ELEMENTS];
    for (int i = 0; i < num_elements; i++) {
        int32_t network_value;
        memcpy(&network_value, packet_buffer + 2 + (i * 4), 4);
        elements[i] = ntohl(network_value);
    }

    insertionSort(elements, num_elements);

    // Prepare and send response
    uint16_t network_num = htons(num_elements);
    memcpy(response_packet, &network_num, 2);
    
    for (int i = 0; i < num_elements; i++) {
        int32_t network_value = htonl(elements[i]);
        memcpy(response_packet + 2 + (i * 4), &network_value, 4);
    }

    int total_size = 2 + (num_elements * 4);
    send(client_socket, response_packet, total_size, 0);
    
    printf("[Thread %lu] Finished processing request\n", pthread_self());
    close(client_socket);
    free(data);
    pthread_exit(NULL);
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

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

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    while(1) {
        client_data *data = malloc(sizeof(client_data));
        data->client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        
        if (data->client_socket < 0) {
            perror("Accept failed");
            free(data);
            continue;
        }

        if (pthread_create(&data->thread_id, NULL, handle_client, (void *)data) < 0) {
            perror("Thread creation failed");
            close(data->client_socket);
            free(data);
            continue;
        }

        // Detach the thread to clean up automatically
        pthread_detach(data->thread_id);
        printf("Created new thread for client\n");
    }

    return 0;
}