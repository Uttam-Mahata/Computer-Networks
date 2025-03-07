
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8888
#define MAX_ELEMENTS 1000
#define MAX_PACKET_SIZE (2 + (MAX_ELEMENTS * 4))  // 2 bytes for count + 4 bytes per element

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char packet_buffer[MAX_PACKET_SIZE];
    uint16_t num_elements;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "192.168.29.22", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server\n");

    // Get number of elements from user
    printf("Enter number of elements (max %d): ", MAX_ELEMENTS);
    scanf("%hu", &num_elements);

    if (num_elements > MAX_ELEMENTS) {
        printf("Too many elements\n");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Prepare packet
    uint16_t network_num = htons(num_elements);
    memcpy(packet_buffer, &network_num, 2);

    // Get elements and pack them
    printf("Enter %d elements:\n", num_elements);
    for (int i = 0; i < num_elements; i++) {
        int value;
        scanf("%d", &value);
        int32_t network_value = htonl(value);
        memcpy(packet_buffer + 2 + (i * 4), &network_value, 4);
    }

    // Send packet
    int total_size = 2 + (num_elements * 4);
    send(sock, packet_buffer, total_size, 0);

    // Receive response
    char response_packet[MAX_PACKET_SIZE];
    
    // Read response size first
    int bytes_read = recv(sock, response_packet, 2, 0);
    if (bytes_read != 2) {
        perror("Failed to read response size");
        close(sock);
        exit(EXIT_FAILURE);
    }

    memcpy(&network_num, response_packet, 2);
    num_elements = ntohs(network_num);

    // Read sorted elements
    bytes_read = recv(sock, response_packet + 2, num_elements * 4, MSG_WAITALL);
    if (bytes_read != num_elements * 4) {
        perror("Failed to read sorted elements");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("\nSorted elements: ");
    for (int i = 0; i < num_elements; i++) {
        int32_t network_value;
        memcpy(&network_value, response_packet + 2 + (i * 4), 4);
        int value = ntohl(network_value);
        printf("%d ", value);
    }
    printf("\n");

    close(sock);
    return 0;
}