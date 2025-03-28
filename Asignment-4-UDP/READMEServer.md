# UDP Packet Forwarding Server

This document provides a detailed explanation of the UDP packet forwarding server implementation (`ServerPacketFwd.c`).

## Overview

The server receives UDP packets, validates them according to specific rules, and then forwards valid packets back to the sender after decrementing the Time-to-Live (TTL) field. For invalid packets, it sends back an error packet with an appropriate error code.

## Data Structures

### Packet Header Structure

```c
typedef struct {
    uint8_t mt;      /* Message Type (1 byte) */
    uint16_t sn;     /* Sequence Number (2 bytes) */
    uint8_t ttl;     /* Time-to-live (1 byte) */
    uint32_t pl;     /* Payload Length (4 bytes) */
} __attribute__((packed)) packet_header_t;
```

- `mt`: Message Type identifier (1 byte)
- `sn`: Sequence Number to track packets (2 bytes)
- `ttl`: Time-to-Live counter (1 byte)
- `pl`: Length of payload in bytes (4 bytes)

The `__attribute__((packed))` ensures that the compiler doesn't add padding between structure members.

### Error Packet Structure

```c
typedef struct {
    uint8_t mt;      /* Message Type (1 byte) */
    uint16_t sn;     /* Sequence Number (2 bytes) */
    uint8_t ec;      /* Error Code (1 byte) */
} __attribute__((packed)) error_packet_t;
```

- `mt`: Always set to 2 for error packets
- `sn`: Echoed from the original packet
- `ec`: Error code indicating the specific validation failure

## Code Walkthrough

### Command Line Arguments

```c
if (argc != 2) {
    fprintf(stderr, "Usage: %s <ServerPort>\n", argv[0]);
    exit(1);
}
int server_port = atoi(argv[1]);
```

The program expects exactly one command line argument: the port number on which the server will listen.

### Socket Creation

```c
int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
```

This creates a UDP socket:
- `AF_INET`: IPv4 addressing
- `SOCK_DGRAM`: Datagram socket (UDP)
- `0`: Default protocol for the socket type

### Server Address Configuration

```c
struct sockaddr_in server_addr, client_addr;
memset(&server_addr, 0, sizeof(server_addr));
memset(&client_addr, 0, sizeof(client_addr));

server_addr.sin_family = AF_INET;
server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
server_addr.sin_port = htons(server_port);
```

- `memset()`: Zero-initializes the address structures
- `sin_family`: Set to IPv4
- `sin_addr.s_addr`: `INADDR_ANY` makes the server listen on all available network interfaces
- `sin_port`: Port number converted to network byte order using `htons()` (host-to-network short)

### Socket Binding

```c
if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("Bind failed");
    close(sockfd);
    exit(1);
}
```

Binds the socket to the specified port and interface. If binding fails, the program prints an error message and exits.

### Main Server Loop

The server runs in an infinite loop, continuously receiving and processing packets:

```c
while (1) {
    // Server operation code
}
```

### Packet Reception

```c
int bytes_received = recvfrom(sockfd, buffer, MAX_BUFFER, 0, 
                    (struct sockaddr *)&client_addr, &client_len);
```

- `recvfrom()`: Receives a UDP datagram
- `buffer`: Storage for the received data
- `MAX_BUFFER`: Maximum buffer size (2048 bytes)
- `client_addr`: Filled with the sender's address information
- `client_len`: Size of the address structure

### Client Information Logging

```c
char client_ip[INET_ADDRSTRLEN];
inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
printf("\n--- Received packet from %s:%d, Size: %d bytes ---\n", 
       client_ip, ntohs(client_addr.sin_port), bytes_received);
```

Converts the client's binary IP address to string format and logs connection details.

### Packet Validation

The server performs several validation checks:

1. **Packet Size Check**:
   ```c
   if (bytes_received < sizeof(packet_header_t)) {
       // Error: Packet too small
       // Send error code 1
   }
   ```
   Ensures the packet is at least as large as the header structure.

2. **Payload Length Check**:
   ```c
   if (bytes_received != sizeof(packet_header_t) + payload_length) {
       // Error: Payload length mismatch
       // Send error code 2
   }
   ```
   Verifies that the received bytes match the claimed payload length plus header size.

3. **Maximum Payload Size Check**:
   ```c
   if (payload_length > 1000) {
       // Error: Payload too large
       // Send error code 3
   }
   ```
   Ensures the payload doesn't exceed 1000 bytes.

4. **TTL Value Check**:
   ```c
   if (packet->ttl % 2 != 0) {
       // Error: TTL is odd
       // Send error code 4
   }
   ```
   Verifies that the TTL value is even.

### Error Handling

For each validation failure, an error packet is constructed and sent back:

```c
error_packet_t error_packet;
error_packet.mt = 2;
error_packet.sn = ntohs(packet->sn);
error_packet.ec = ERROR_CODE;  

sendto(sockfd, &error_packet, sizeof(error_packet_t), 0,
      (struct sockaddr *)&client_addr, client_len);
```

### Packet Forwarding

For valid packets, the TTL is decremented and the packet is sent back:

```c
packet->ttl--;
sendto(sockfd, buffer, bytes_received, 0, 
       (struct sockaddr *)&client_addr, client_len);
```

## Error Codes Reference

| Error Code | Description |
|------------|-------------|
| 1 | Packet too small (< header size) |
| 2 | Payload length mismatch |
| 3 | Payload too large (> 1000 bytes) |
| 4 | TTL is odd |

## Running the Server

Compile the program:
```bash
gcc -o ServerPacketFwd ServerPacketFwd.c
```

Run the server:
```bash
./ServerPacketFwd <port_number>
```
Replace `<port_number>` with the desired port number.
