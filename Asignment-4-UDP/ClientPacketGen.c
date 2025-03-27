#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <time.h>

#define MAX_PACKET_SIZE 2048
#define HEADER_SIZE 8  // MT(1) + SN(2) + TTL(1) + PL(4)
#define ERROR_PACKET_SIZE 4  // MT(1) + SN(2) + EC(1)

// Error codes
#define ERR_TOO_SMALL 1
#define ERR_PAYLOAD_INCONSISTENT 2
#define ERR_TOO_LARGE_PAYLOAD 3
#define ERR_TTL_NOT_EVEN 4

// Function to calculate time difference in milliseconds
double time_diff_ms(struct timeval *start, struct timeval *end) {
    return (end->tv_sec - start->tv_sec) * 1000.0 + 
           (end->tv_usec - start->tv_usec) / 1000.0;
}

int main(int argc, char *argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Usage: %s <ServerIP> <ServerPort> <P> <TTL> <NumPackets>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);
    int payload_length = atoi(argv[3]);
    int ttl = atoi(argv[4]);
    int num_packets = atoi(argv[5]);
    
    // Validate input parameters
    if (payload_length < 100 || payload_length > 1000) {
        fprintf(stderr, "Payload length P must be between 100 and 1000 bytes\n");
        exit(EXIT_FAILURE);
    }
    
    if (ttl < 2 || ttl > 20 || ttl % 2 != 0) {
        fprintf(stderr, "TTL must be an even number between 2 and 20\n");
        exit(EXIT_FAILURE);
    }
    
    if (num_packets < 1 || num_packets > 50) {
        fprintf(stderr, "Number of packets must be between 1 and 50\n");
        exit(EXIT_FAILURE);
    }
    
    // Create socket
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Set socket timeout
    struct timeval timeout;
    timeout.tv_sec = 5;  // 5 seconds timeout
    timeout.tv_usec = 0;
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("Setsockopt failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    // Server address structure
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    
    // Convert IP address from string to binary
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    unsigned char packet[MAX_PACKET_SIZE];
    unsigned char response[MAX_PACKET_SIZE];
    double total_rtt = 0.0;
    int successful_packets = 0;
    
    // Seed random number generator
    srand(time(NULL));
    
    printf("Sending %d packets to %s:%d\n", num_packets, server_ip, server_port);
    printf("Payload length: %d bytes, TTL: %d\n\n", payload_length, ttl);
    
    // Generate and send packets
    for (int i = 0; i < num_packets; i++) {
        // Prepare packet
        packet[0] = 1;  // MT = 1 (payload packet)
        
        // Set sequence number (2 bytes)
        unsigned short seq_num = (unsigned short)i;
        memcpy(&packet[1], &seq_num, 2);
        
        packet[3] = ttl;  // TTL
        
        // Set payload length (4 bytes)
        memcpy(&packet[4], &payload_length, 4);
        
        // Fill payload with random data
        for (int j = 0; j < payload_length; j++) {
            packet[HEADER_SIZE + j] = (unsigned char)(rand() % 256);
        }
        
        socklen_t server_len = sizeof(server_addr);
        struct timeval start_time, end_time;
        
        // Send packet and record time
        gettimeofday(&start_time, NULL);
        
        if (sendto(sockfd, packet, HEADER_SIZE + payload_length, 0, 
                  (const struct sockaddr *)&server_addr, server_len) < 0) {
            perror("Send failed");
            continue;
        }
        
        // Receive response
        int n = recvfrom(sockfd, response, MAX_PACKET_SIZE, 0,
                        (struct sockaddr *)&server_addr, &server_len);
        
        gettimeofday(&end_time, NULL);
        
        if (n < 0) {
            perror("Receive timeout");
            printf("Packet %d: No response received (timeout)\n", i);
            continue;
        }
        
        // Check if it's an error packet
        if (n == ERROR_PACKET_SIZE && response[0] == 2) {
            unsigned short resp_seq_num;
            memcpy(&resp_seq_num, &response[1], 2);
            
            printf("Packet %d: Error - ", resp_seq_num);
            
            switch (response[3]) {
                case ERR_TOO_SMALL:
                    printf("TOO SMALL PACKET RECEIVED\n");
                    break;
                case ERR_PAYLOAD_INCONSISTENT:
                    printf("PAYLOAD LENGTH AND PAYLOAD INCONSISTENT\n");
                    break;
                case ERR_TOO_LARGE_PAYLOAD:
                    printf("TOO LARGE PAYLOAD LENGTH\n");
                    break;
                case ERR_TTL_NOT_EVEN:
                    printf("TTL VALUE IS NOT EVEN\n");
                    break;
                default:
                    printf("Unknown error code: %d\n", response[3]);
            }
            continue;
        }
        
        // Normal response - calculate RTT
        double rtt = time_diff_ms(&start_time, &end_time);
        total_rtt += rtt;
        successful_packets++;
        
        unsigned short resp_seq_num;
        memcpy(&resp_seq_num, &response[1], 2);
        
        printf("Packet %d: RTT = %.3f ms\n", resp_seq_num, rtt);
    }
    
    // Calculate and display average RTT
    if (successful_packets > 0) {
        double avg_rtt = total_rtt / successful_packets;
        printf("\nSuccessful packets: %d/%d\n", successful_packets, num_packets);
        printf("Average RTT: %.3f ms\n", avg_rtt);
    } else {
        printf("\nNo successful packet exchanges completed\n");
    }
    
    close(sockfd);
    return 0;
}
