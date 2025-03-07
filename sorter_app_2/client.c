#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 3490
#define MAX_ELEMENTS 1000
#define MAX_PACKET_SIZE (2 + (MAX_ELEMENTS * 4))

int connect_to_server() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "10.2.65.33", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(sock);
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        return -1;
    }

    return sock;
}

int main() {
    int sock;
    char packet_buffer[MAX_PACKET_SIZE];
    uint16_t num_elements;

    // Connect to server
    if ((sock = connect_to_server()) < 0) {
        exit(EXIT_FAILURE);
    }

    printf("Connected to server.\n");

    // Get number of elements from user
    do {
        printf("Enter number of elements (1-%d): ", MAX_ELEMENTS);
        if (scanf("%hu", &num_elements) != 1) {
            printf("Invalid input\n");
            while (getchar() != '\n');  // Clear input buffer
            continue;
        }
    } while (num_elements < 1 || num_elements > MAX_ELEMENTS);

    // Prepare packet
    uint16_t network_num = htons(num_elements);
    memcpy(packet_buffer, &network_num, 2);

    // Get elements and pack them
    printf("Enter %d elements:\n", num_elements);
    for (int i = 0; i < num_elements; i++) {
        int value;
        printf("Element %d: ", i + 1);
        while (scanf("%d", &value) != 1) {
            printf("Invalid input. Enter element %d again: ", i + 1);
            while (getchar() != '\n');  // Clear input buffer
        }
        int32_t network_value = htonl(value);
        memcpy(packet_buffer + 2 + (i * 4), &network_value, 4);
    }

    // Send packet
    int total_size = 2 + (num_elements * 4);
    if (send(sock, packet_buffer, total_size, 0) != total_size) {
        perror("Failed to send data");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Receive response
    char response_packet[MAX_PACKET_SIZE];
    
    // Read response size
    if (recv(sock, response_packet, 2, MSG_WAITALL) != 2) {
        perror("Failed to read response size");
        close(sock);
        exit(EXIT_FAILURE);
    }

    uint16_t received_num;
    memcpy(&received_num, response_packet, 2);
    received_num = ntohs(received_num);

    // Read sorted elements
    if (recv(sock, response_packet + 2, received_num * 4, MSG_WAITALL) != received_num * 4) {
        perror("Failed to read sorted elements");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Display sorted elements
    printf("\nSorted elements: ");
    for (int i = 0; i < received_num; i++) {
        int32_t network_value;
        memcpy(&network_value, response_packet + 2 + (i * 4), 4);
        int value = ntohl(network_value);
        printf("%d ", value);
    }
    printf("\n");

    close(sock);
    return 0;
}