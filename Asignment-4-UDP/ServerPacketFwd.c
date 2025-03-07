/*Server.c*/

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

// Message Type (MT): Represents type of the message. The value of this field will be set 1,
// to represent it as a payload packet.
// Sequence Number (SN): Uniquely identifies individual packet.
// Time-to-live field (TTL): A non-negative even integer with an initial value of T.
// Payload Length (PL): The length of the payload bytes P.
// Payload Bytes (PB): Arbitrary payload bytes of size P bytes.



typedef struct pdu {
    int message_type[2]; /*Represents type of message. value = 1 to represent payload packet.; value =2 if server founds it as error packet*/
    uid_t seq_no; /*Unique Identifier*/
    int ttl; /*A non negative even integer initialized with value T*/
    int payload; /*Length of Payload Bytes = P */
    int payload_bytes; /*Arbitrary Payload Bytes of size P*/
} pdu;




int main(int argc, char * argv[]) {

   if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

int server_fd, client_fd;
struct sockaddr_in server_addr, client_addr;


if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1])); /* Convert port to network byte order */

if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

int addr_len = sizeof(client_addr);
pdu packet;

while (1) {
    ssize_t bytes_received = recvfrom(server_fd, &packet, sizeof(packet), 0, (struct sockaddr *)&client_addr, &addr_len);
    if (bytes_received < 0) {
        perror("Packet receiving failed");
        continue;
    }

    // Check for packet consistency
    if (packet.payload < 0 || packet.payload > 1024) {
        printf("Error: TOO SMALL PACKET RECEIVED\n");
        continue;
    }

    if (packet.ttl % 2 != 0) {
        printf("Error: TTL VALUE IS NOT EVEN\n");
        continue;
    }

    if (packet.ttl == 0) {
        printf("Error: TTL VALUE IS ZERO\n");
        continue;
    }

    if (packet.message_type[0] != 1) {
        printf("Error: INVALID MESSAGE TYPE\n");
        continue;
    }

    // Decrement TTL and send back to client
    packet.ttl--;
    ssize_t bytes_sent = sendto(server_fd, &packet, sizeof(packet), 0, (struct sockaddr *)&client_addr, addr_len);
    if (bytes_sent < 0) {
        perror("Packet sending failed");
        continue;
    }

    printf("Packet sent back to client successfully\n");
    printf("Packet details: SN=%d, TTL=%d, PL=%d, PB=%d\n", packet.seq_no, packet.ttl, packet.payload, packet.payload_bytes);
    // Check for termination condition
    if (packet.ttl == 0) {
        printf("Terminating connection due to TTL expiration\n");
        break;
    }
    if (packet.payload == 0) {
        printf("Terminating connection due to payload length of zero\n");
        break;
    }
    if (packet.payload_bytes == 0) {
        printf("Terminating connection due to payload bytes of zero\n");
        break;
    }





}
