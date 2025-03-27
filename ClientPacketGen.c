#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#define MAX_BUFFER 2048
#define MAX_PACKETS 50

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

/* Calculation of RTT:
   The RTT is calculated as the difference between the time when the packet was sent
   and the time when the response was received. The time is measured in milliseconds.
   The function takes two timeval structures as input and returns the RTT in milliseconds.

*/
double calculate_rtt(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
}



int main(int argc, char *argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Usage: %s <ServerIP> <ServerPort> <P> <TTL> <NumPackets>\n", argv[0]);
        exit(1);
    }

    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);
    int payload_size = atoi(argv[3]);
    int ttl = atoi(argv[4]);
    int num_packets = atoi(argv[5]);

    /*
         Input Validation:
         The program checks if the input parameters are within the specified ranges.
         If any parameter is out of range, an error message is printed and the program exits.
         
         Payload size (P) must be between 100 and 1000 bytes.
         TTL must be an even number between 2 and 20.
         NumPackets must be between 1 and 50.
    */
    
    if (payload_size < 100 || payload_size > 1000) {
        fprintf(stderr, "Payload size (P) must be between 100 and 1000\n");
        exit(1);
    }
    
    if (ttl < 2 || ttl > 20 || ttl % 2 != 0) {
        fprintf(stderr, "TTL must be an even number between 2 and 20\n");
        exit(1);
    }
    
    if (num_packets < 1 || num_packets > 50) {
        fprintf(stderr, "NumPackets must be between 1 and 50\n");
        exit(1);
    }

/*
        Socket Creation:
        A UDP socket is created using the socket() function. The socket is configured to
        receive packets with a timeout of 2 seconds using setsockopt().
    */
    
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    
// Setting timeout for receiving packets
    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    /*
        Setting socket options:
        The socket is configured to have a receive timeout of 2 seconds using setsockopt().
        This means that if no response is received within this time, the recvfrom() call will
        return with an error.
    */
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt failed");
        close(sockfd);
        exit(1);
    }
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(sockfd);
        exit(1);
    }
    
    // Allocate buffer for packet
    unsigned char *buffer = malloc(sizeof(packet_header_t) + payload_size);
    if (buffer == NULL) {
        perror("Memory allocation failed");
        close(sockfd);
        exit(1);
    }
    
    // Preparing packet header
    packet_header_t *packet = (packet_header_t *)buffer;
    packet->mt = 1;  // Payload packet
    packet->ttl = ttl;
    packet->pl = htonl(payload_size);

/* Random Payload Generation: 
   Fill the payload with random data. The payload size is specified by the user.
   The payload starts after the header, so we calculate its position in the buffer.
   The payload is filled with random bytes generated using rand() function.

*/    
    unsigned char *payload = buffer + sizeof(packet_header_t);
    for (int i = 0; i < payload_size; i++) {
        payload[i] = rand() % 256;
    }
    
    double rtt_values[MAX_PACKETS] = {0};
    int successful_packets = 0;
    
    for (int i = 0; i < num_packets; i++) {
        packet->sn = htons(i);  // Set sequence number
        
        // Record start time
        struct timeval start_time, end_time;
        gettimeofday(&start_time, NULL);

        /*
            Sending the packet to the server using sendto() function.
            The packet is sent to the server address specified in server_addr.
            The size of the packet is calculated as the size of the header plus the payload size.
        */
        
        sendto(sockfd, buffer, sizeof(packet_header_t) + payload_size, 0,
               (struct sockaddr *)&server_addr, sizeof(server_addr));
        
        unsigned char recv_buffer[MAX_BUFFER];
        socklen_t server_len = sizeof(server_addr);
        int bytes_received = recvfrom(sockfd, recv_buffer, MAX_BUFFER, 0,
                                     (struct sockaddr *)&server_addr, &server_len);
        
        gettimeofday(&end_time, NULL);
        
        if (bytes_received < 0) {
            printf("Packet %d: Timeout or error\n", i);
            continue;
        }
        
        if (bytes_received == sizeof(error_packet_t)) {
            error_packet_t *error = (error_packet_t *)recv_buffer;
            if (error->mt == 2) {
                printf("Packet %d: Error - ", ntohs(error->sn));
                switch (error->ec) {
                    case 1:
                        printf("TOO SMALL PACKET RECEIVED\n");
                        break;
                    case 2:
                        printf("PAYLOAD LENGTH AND PAYLOAD INCONSISTENT\n");
                        break;
                    case 3:
                        printf("TOO LARGE PAYLOAD LENGTH\n");
                        break;
                    case 4:
                        printf("TTL VALUE IS NOT EVEN\n");
                        break;
                    default:
                        printf("Unknown error code: %d\n", error->ec);
                }
                continue;
            }
        }
        
        double rtt = calculate_rtt(start_time, end_time);
        printf("Packet %d: RTT = %.3f ms\n", i, rtt);
        
        rtt_values[successful_packets++] = rtt;
    }
    
    if (successful_packets > 0) {
        double total_rtt = 0;
        for (int i = 0; i < successful_packets; i++) {
            total_rtt += rtt_values[i];
        }
        
        double avg_rtt = total_rtt / successful_packets;
        printf("\nSummary:\n");
        printf("Total packets sent: %d\n", num_packets);
        printf("Successful responses: %d\n", successful_packets);
        printf("Average RTT: %.3f ms\n", avg_rtt);
    } else {
        printf("\nNo successful packet exchanges\n");
    }
    
    free(buffer);
    close(sockfd);
    return 0;
}
