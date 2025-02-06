#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s <ip> <port> <num1> [num2] [num3] ...\n", argv[0]);
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

    // Send number of elements (2 bytes)
    uint16_t num_elements = argc - 3;
    uint16_t send_size = htons(num_elements);
    if (send(sock_fd, &send_size, sizeof(uint16_t), 0) != sizeof(uint16_t)) {
        perror("Error sending number of elements");
        close(sock_fd);
        exit(1);
    }

    // Send each number (4 bytes each)
    printf("Sending %d numbers: ", num_elements);
    for(int i = 3; i < argc; i++) {
        uint32_t num = htonl(atoi(argv[i]));
        if (send(sock_fd, &num, sizeof(uint32_t), 0) != sizeof(uint32_t)) {
            perror("Error sending number");
            close(sock_fd);
            exit(1);
        }
        printf("%d ", atoi(argv[i]));
    }
    printf("\n");

    // Receive sorted array
    uint16_t received_size;
    if (recv(sock_fd, &received_size, sizeof(uint16_t), MSG_WAITALL) != sizeof(uint16_t)) {
        perror("Error receiving result size");
        close(sock_fd);
        exit(1);
    }
    received_size = ntohs(received_size);

    printf("Receiving %d sorted numbers: ", received_size);
    for(int i = 0; i < received_size; i++) {
        uint32_t num;
        if (recv(sock_fd, &num, sizeof(uint32_t), MSG_WAITALL) != sizeof(uint32_t)) {
            perror("Error receiving sorted number");
            close(sock_fd);
            exit(1);
        }
        printf("%d ", ntohl(num));
    }
    printf("\n");

    close(sock_fd);
    return 0;
}
