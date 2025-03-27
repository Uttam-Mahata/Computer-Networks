#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_PACKET_SIZE 2048
#define HEADER_SIZE 8  // MT(1) + SN(2) + TTL(1) + PL(4)
#define ERROR_PACKET_SIZE 4  // MT(1) + SN(2) + EC(1)

// Error codes
#define ERR_TOO_SMALL 1
#define ERR_PAYLOAD_INCONSISTENT 2
#define ERR_TOO_LARGE_PAYLOAD 3
#define ERR_TTL_NOT_EVEN 4

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <ServerPort>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    
    // Create socket
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Server address structure
    struct sockaddr_in server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    // Bind socket to address and port
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    printf("Server is running on port %d...\n", port);
    
    unsigned char buffer[MAX_PACKET_SIZE];
    unsigned char response[MAX_PACKET_SIZE];
    
    while (1) {
        socklen_t client_len = sizeof(client_addr);
        
        // Receive packet from client
        int n = recvfrom(sockfd, buffer, MAX_PACKET_SIZE, 0, 
                         (struct sockaddr *)&client_addr, &client_len);
        
        if (n <= 0) continue;  // Skip if no data received
        
        // Check if packet is at least as large as the header
        if (n < HEADER_SIZE) {
            // Prepare error packet: Too small packet
            response[0] = 2;  // MT = 2 (error)
            memcpy(&response[1], &buffer[1], 2);  // Copy SN
            response[3] = ERR_TOO_SMALL;
            
            sendto(sockfd, response, ERROR_PACKET_SIZE, 0,
                  (const struct sockaddr *)&client_addr, client_len);
            continue;
        }
        
        // Extract payload length from the packet
        int payload_length;
        memcpy(&payload_length, &buffer[4], 4);
        
        // Check if packet size matches payload length + header
        if (n != HEADER_SIZE + payload_length) {
            // Prepare error packet: Payload inconsistent
            response[0] = 2;  // MT = 2 (error)
            memcpy(&response[1], &buffer[1], 2);  // Copy SN
            response[3] = ERR_PAYLOAD_INCONSISTENT;
            
            sendto(sockfd, response, ERROR_PACKET_SIZE, 0,
                  (const struct sockaddr *)&client_addr, client_len);
            continue;
        }
        
        // Check if payload is too large
        if (payload_length > 1000) {
            // Prepare error packet: Too large payload
            response[0] = 2;  // MT = 2 (error)
            memcpy(&response[1], &buffer[1], 2);  // Copy SN
            response[3] = ERR_TOO_LARGE_PAYLOAD;
            
            sendto(sockfd, response, ERROR_PACKET_SIZE, 0,
                  (const struct sockaddr *)&client_addr, client_len);
            continue;
        }
        
        // Check if TTL is even
        if (buffer[3] % 2 != 0) {
            // Prepare error packet: TTL not even
            response[0] = 2;  // MT = 2 (error)
            memcpy(&response[1], &buffer[1], 2);  // Copy SN
            response[3] = ERR_TTL_NOT_EVEN;
            
            sendto(sockfd, response, ERROR_PACKET_SIZE, 0,
                  (const struct sockaddr *)&client_addr, client_len);
            continue;
        }
        
        // If all checks pass, decrement TTL and forward packet back
        memcpy(response, buffer, n);
        response[3]--;  // Decrement TTL
        
        sendto(sockfd, response, n, 0,
              (const struct sockaddr *)&client_addr, client_len);
    }
    
    close(sockfd);
    return 0;
}
