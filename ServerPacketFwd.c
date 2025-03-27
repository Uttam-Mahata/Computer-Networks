#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_BUFFER 2048

typedef struct {
    uint8_t mt;      /* Message Type (1 byte) */
    uint16_t sn;     /* Sequence Number (2 bytes) */
    uint8_t ttl;     /* Time-to-live (1 byte) */
    uint32_t pl;     /* Payload Length (4 bytes) */
} __attribute__((packed)) packet_header_t;

typedef struct {
    uint8_t mt;      /* Message Type (1 byte) */
    uint16_t sn;     /* Sequence Number (2 bytes) */
    uint8_t ec;      /*Error Code (1 byte)*/
} __attribute__((packed)) error_packet_t;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <ServerPort>\n", argv[0]);
        exit(1);
    }

    int server_port = atoi(argv[1]);
    
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    
    struct sockaddr_in server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(server_port);
    
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(1);
    }
    
    unsigned char buffer[MAX_BUFFER];
    socklen_t client_len = sizeof(client_addr);
    
    printf("Server is running on port %d...\n", server_port);
    
    while (1) {
        int bytes_received = recvfrom(sockfd, buffer, MAX_BUFFER, 0, 
                            (struct sockaddr *)&client_addr, &client_len);
        
        if (bytes_received < 0) {
            perror("Receive failed");
            continue;
        }
        
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("\n--- Received packet from %s:%d, Size: %d bytes ---\n", 
               client_ip, ntohs(client_addr.sin_port), bytes_received);
        
        // Packet Validation
        if (bytes_received < sizeof(packet_header_t)) {
            // Packet too small
            printf("Error: Packet too small (< %lu bytes)\n", sizeof(packet_header_t));
            error_packet_t error_packet;
            error_packet.mt = 2;
            error_packet.sn = ((packet_header_t *)buffer)->sn;
            error_packet.ec = 1;

            sendto(sockfd, &error_packet, sizeof(error_packet_t), 0,
                  (struct sockaddr *)&client_addr, client_len);
            printf("Sent error packet: MT=%d, SN=%d, EC=%d (Packet too small)\n", 
                   error_packet.mt, error_packet.sn, error_packet.ec);
            continue;
        }
        
        packet_header_t *packet = (packet_header_t *)buffer;
        uint32_t payload_length = ntohl(packet->pl);
        
        printf("Packet Details: MT=%d, SN=%d, TTL=%d, PL=%u\n", 
               packet->mt, ntohs(packet->sn), packet->ttl, payload_length);
        
        // Checking if received bytes match header + claimed payload size
        if (bytes_received != sizeof(packet_header_t) + payload_length) {
            printf("Error: Payload length mismatch (Expected %lu+%u=%lu bytes, Got %d bytes)\n", 
                   sizeof(packet_header_t), payload_length, sizeof(packet_header_t) + payload_length, bytes_received);
            error_packet_t error_packet;
            error_packet.mt = 2;
            error_packet.sn = ntohs(packet->sn);
            error_packet.ec = 2;  
            
            sendto(sockfd, &error_packet, sizeof(error_packet_t), 0,
                  (struct sockaddr *)&client_addr, client_len);
            printf("Sent error packet: MT=%d, SN=%d, EC=%d (Payload length mismatch)\n", 
                   error_packet.mt, error_packet.sn, error_packet.ec);
            continue;
        }
        
        // Checking if payload is too large (> 1000)
        if (payload_length > 1000) {
            printf("Error: Payload too large (%u > 1000 bytes)\n", payload_length);
            error_packet_t error_packet;
            error_packet.mt = 2;
            error_packet.sn = ntohs(packet->sn);
            error_packet.ec = 3; 
            
            sendto(sockfd, &error_packet, sizeof(error_packet_t), 0,
                  (struct sockaddr *)&client_addr, client_len);
            printf("Sent error packet: MT=%d, SN=%d, EC=%d (Payload too large)\n", 
                   error_packet.mt, error_packet.sn, error_packet.ec);
            continue;
        }
        
        // Checking if TTL is even
        if (packet->ttl % 2 != 0) {
            printf("Error: TTL is odd (%d)\n", packet->ttl);
            error_packet_t error_packet;
            error_packet.mt = 2;
            error_packet.sn = ntohs(packet->sn);
            error_packet.ec = 4;  
            
            sendto(sockfd, &error_packet, sizeof(error_packet_t), 0,
                  (struct sockaddr *)&client_addr, client_len);
            printf("Sent error packet: MT=%d, SN=%d, EC=%d (TTL is odd)\n", 
                   error_packet.mt, error_packet.sn, error_packet.ec);
            continue;
        }
        
        // Decrement TTL and send back
        packet->ttl--;
        printf("Valid packet! Decrementing TTL from %d to %d and forwarding\n", packet->ttl + 1, packet->ttl);
        
        sendto(sockfd, buffer, bytes_received, 0, 
               (struct sockaddr *)&client_addr, client_len);
        printf("Forwarded packet to client (%d bytes)\n", bytes_received);
    }
    
    close(sockfd);
    return 0;
}
