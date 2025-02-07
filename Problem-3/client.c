#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8082
#define SERVER_IP "10.2.1.40"
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("invalid address");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection failed");
        exit(EXIT_FAILURE);
    }

    int n;
    printf("Enter number of elements: ");
    scanf("%d", &n);

    int32_t* elements = (int32_t*)malloc(n * sizeof(int32_t));
    printf("Enter elements: ");
    for (int i = 0; i < n; ++i) 
        scanf("%d", &elements[i]);

    uint8_t* send_buffer = (uint8_t*)malloc(BUFFER_SIZE);
    *((uint16_t*)send_buffer) = htons(n);
    for (int i = 0; i < n; ++i) {
        *((int32_t*)(send_buffer + 2 + i*4)) = htonl(elements[i]);
    }

    int total_sent = 0;
    int bytes_to_send = 2 + n * 4;
    while (total_sent < bytes_to_send) {
        int sent = send(sock, send_buffer + total_sent, bytes_to_send - total_sent, 0);
        if (sent <= 0) break;
        total_sent += sent;
    }

    uint8_t* recv_buffer = (uint8_t*)malloc(BUFFER_SIZE);
    int bytes_read = read(sock, recv_buffer, 2);
    if (bytes_read != 2) {
        fprintf(stderr, "Failed to read count\n");
        close(sock);
        free(elements);
        free(send_buffer);
        free(recv_buffer);
        return -1;
    }

    uint16_t num_elements = ntohs(*((uint16_t*)recv_buffer));
    int total_received = 2;
    int required = num_elements * 4;

    while (total_received < required + 2) {
        bytes_read = read(sock, recv_buffer + total_received, BUFFER_SIZE - total_received);
        if (bytes_read <= 0) break;
        total_received += bytes_read;
    }

    printf("Sorted elements: ");
    for (int i = 0; i < num_elements; ++i) {
        int32_t value = ntohl(*((int32_t*)(recv_buffer + 2 + i*4)));
        printf("%d ", value);
    }
    printf("\n");

    free(elements);
    free(send_buffer);
    free(recv_buffer);
    close(sock);
    return 0;
}
