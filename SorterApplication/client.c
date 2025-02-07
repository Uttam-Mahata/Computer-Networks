#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    int sock_fd;
    struct sockaddr_in server_addr;

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));

    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(1);
    }

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    // Get number of elements from user
    uint16_t num_elements;
    printf("Enter the number of elements: ");
    scanf("%hu", &num_elements);

    // Allocate array for all numbers
    int *numbers = malloc(num_elements * sizeof(int));
    if (!numbers) {
        perror("Memory allocation failed");
        close(sock_fd);
        exit(1);
    }

    // Get all numbers at once
    printf("Enter %d numbers:\n", num_elements);
    for(int i = 0; i < num_elements; i++) {
        scanf("%d", &numbers[i]);
    }

    // Send number of elements (2 bytes)
    uint16_t send_size = htons(num_elements);
    if (send(sock_fd, &send_size, sizeof(uint16_t), 0) != sizeof(uint16_t)) {
        perror("Error sending number of elements");
        close(sock_fd);
        exit(1);
    }

    // Send all numbers in one go
    for(int i = 0; i < num_elements; i++) {
        uint32_t network_num = htonl(numbers[i]);
        numbers[i] = network_num;
    }
    if (send(sock_fd, numbers, num_elements * sizeof(uint32_t), 0) != num_elements * sizeof(uint32_t)) {
        perror("Error sending numbers");
        free(numbers);
        close(sock_fd);
        exit(1);
    }

    // Receive sorted array
    if (recv(sock_fd, numbers, num_elements * sizeof(uint32_t), MSG_WAITALL) != num_elements * sizeof(uint32_t)) {
        perror("Error receiving sorted numbers");
        free(numbers);
        close(sock_fd);
        exit(1);
    }

    printf("Sorted numbers: ");
    for(int i = 0; i < num_elements; i++) {
        printf("%d ", ntohl(numbers[i]));
    }
    printf("\n");

    free(numbers);
    close(sock_fd);
    return 0;
}
