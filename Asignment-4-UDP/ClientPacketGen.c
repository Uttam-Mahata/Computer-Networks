#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 3490

typedef struct pdu {
int message_type[2]; /*Represents type of message. value = 1 to represent payload packet.; value =2 if server founds it as error packet*/
uid_t seq_no; /*Unique Identifier*/
int ttl; /*A non negative even integer initialized with value T*/
int payload; /*Length of Payload Bytes = P */
int payload_bytes; /*Arbitrary Payload Bytes of size P*/
} pdu;

int main(int argc, char *argv[]) {
if (argc != 5) {
        printf("Usage: %s <ip> <port> <Size of Payload Bytes> <TTL> <NumOfPackets>\n", argv[0]);
        exit(1);
    }
int sock_fd;
struct sockaddr_in server_addr;

/*Create Socket for DATAGRAM */
if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
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

//Sending pdu to server
pdu packet;

packet.message_type[0] = 1;
packet.message_type[1] = 0;
packet.seq_no = 1;
packet.ttl = atoi(argv[4]);
packet.payload = atoi(argv[3]);
packet.payload_bytes = 0;

int num_packets = atoi(argv[5]);

for (int i = 0; i < num_packets; i++) {
	if (sendto(sock_fd, &packet, sizeof(packet), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
	    perror("Packet sending failed");
	    exit(1);
	}
    printf("Packet %d sent successfully\n", i + 1);

    // Get Same Datagram back from server
    char buffer[sizeof(packet)];
    socklen_t addr_len = sizeof(server_addr);
    if (recvfrom(sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&server_addr, &addr_len) < 0) {
        perror("Packet receiving failed");
        exit(1);
    }

    // Process the received packet
    pdu *received_packet = (pdu *)buffer;
    if (received_packet->message_type[0] == 2) {
        printf("Error packet received: %d\n", received_packet->message_type[1]);
        switch (received_packet->message_type[1]) {
            case 1:
                printf("Error: TOO SMALL PACKET RECEIVED\n");
                break;
            case 2:
                printf("Error: PAYLOAD LENGTH AND PAYLOAD INCONSISTENT\n");
                break;
            case 3:
                printf("Error: TOO LARGE PAYLOAD LENGTH\n");
                break;
            case 4:
                printf("Error: TTL VALUE IS NOT EVEN\n");
                break;
            default:
                printf("Unknown error code\n");
        }
        break;
    }
    printf("Received packet with seq_no: %d\n", received_packet->seq_no);
    printf("TTL: %d\n", received_packet->ttl);
    printf("Payload: %d\n", received_packet->payload);
    printf("Payload bytes: %d\n", received_packet->payload_bytes);
    printf("Packet %d received successfully\n", i + 1);
    packet.seq_no++;
    packet.payload_bytes += packet.payload;
    packet.payload = 0;
    packet.ttl -= 2;

    if (packet.ttl < 0) {
        printf("TTL expired, stopping packet generation.\n");
        break;
    }

close(sock_fd);

}
