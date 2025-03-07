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

typedef struct pdu {
int message_type[2]; /*Represents type of message. value = 1 to represent payload packet.; value =2 if server founds it as error packet*/
uid_t seq_no; /*Unique Identifier*/
int ttl; /*A non negative even integer initialized with value T*/
int payload; /*Length of Payload Bytes*/
int payload_bytes; /*Arbitrary Payload Bytes*/

} pdu;

// Error Code (EC): Error codes can be of these types -
// 1: TOO SMALL PACKET RECEIVED
// 2: PAYLOAD LENGTH AND PAYLOAD INCONSISTENT
// 3: TOO LARGE PAYLOAD LENGTH
// 4: TTL VALUE IS NOT EVEN


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



}
