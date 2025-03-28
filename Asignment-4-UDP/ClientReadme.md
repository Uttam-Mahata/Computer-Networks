# UDP Packet Generator Client

This document provides a detailed explanation of the UDP packet generator client implementation (`ClientPacketGen.c`).

## Overview

The client sends a specified number of UDP packets to a server, measures their round-trip time (RTT), and handles responses including error packets. It validates user inputs, constructs packet headers correctly, generates random payload data, and provides statistical summary of the communication.

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

- `mt`: Identifies the packet type (set to 1 for regular packets)
- `sn`: Tracks the sequence of packets sent
- `ttl`: Time-to-Live counter that the server will decrement
- `pl`: Specifies the payload length in bytes

The `__attribute__((packed))` ensures the compiler doesn't add padding bytes between structure members, maintaining exact byte layout required for network protocols.

### Error Packet Structure

```c
typedef struct {
    uint8_t mt;      /* Message Type (1 byte) */
    uint16_t sn;     /* Sequence Number (2 bytes) */
    uint8_t ec;      /* Error Code (1 byte) */
} __attribute__((packed)) error_packet_t;
```

- `mt`: Always 2 for error packets (received from server)
- `sn`: Echoes the sequence number of the original packet
- `ec`: Error code indicating what validation check failed

## RTT Calculation Function

```c
double calculate_rtt(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
}
```

This function:
- Takes two `timeval` structures representing packet send and receive times
- Converts the time difference to milliseconds by:
  - Multiplying seconds difference by 1000 to get milliseconds
  - Converting microseconds difference to milliseconds by dividing by 1000
  - Adding both components to get the total RTT in milliseconds
- Provides a high-precision measurement of network latency

## Code Walkthrough

### Command Line Arguments

```c
if (argc != 6) {
    fprintf(stderr, "Usage: %s <ServerIP> <ServerPort> <P> <TTL> <NumPackets>\n", argv[0]);
    exit(1);
}
```

The program requires exactly 5 arguments:
1. `ServerIP`: IP address of the target server
2. `ServerPort`: Port number the server is listening on
3. `P`: Payload size in bytes
4. `TTL`: Time-to-Live value
5. `NumPackets`: Number of packets to send

### Input Validation

```c
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
```

The client enforces specific constraints for each parameter:
- Payload size: Between 100-1000 bytes (prevents unnecessary network congestion while ensuring adequately sized packets)
- TTL: Must be even and between 2-20 (server is designed to accept only even TTL values)
- Packet count: Limited to 1-50 (prevents unintentional flooding)

### Socket Creation and Configuration

```c
int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
```

Creates a UDP socket:
- `AF_INET`: Uses IPv4 addressing
- `SOCK_DGRAM`: Datagram socket (UDP protocol)
- `0`: Default protocol for the socket type

```c
struct timeval timeout;
timeout.tv_sec = 2;
timeout.tv_usec = 0;

if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
    perror("setsockopt failed");
    close(sockfd);
    exit(1);
}
```

Sets a 2-second receive timeout on the socket:
- `SOL_SOCKET`: Socket level option
- `SO_RCVTIMEO`: Option name for receive timeout
- This prevents the client from hanging indefinitely if the server doesn't respond
- Critical for reliable operation in real-world networks where packets may be lost

### Server Address Configuration

```c
struct sockaddr_in server_addr;
memset(&server_addr, 0, sizeof(server_addr));

server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(server_port);
if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
    perror("Invalid address");
    close(sockfd);
    exit(1);
}
```

Configures the target server's address:
- `memset()`: Zero-initializes the address structure for safety
- `sin_family`: Sets IPv4 addressing
- `sin_port`: Converts port number to network byte order (big-endian)
- `inet_pton()`: Converts human-readable IP address to binary format
- Validates the provided IP address to ensure it's correctly formatted

### Memory Allocation

```c
unsigned char *buffer = malloc(sizeof(packet_header_t) + payload_size);
if (buffer == NULL) {
    perror("Memory allocation failed");
    close(sockfd);
    exit(1);
}
```

Dynamically allocates memory for the packet:
- Size is exactly what's needed: header size + payload size
- Checks for allocation failure, as memory availability isn't guaranteed
- Ensures no buffer overflow will occur during payload generation

### Packet Header Initialization

```c
packet_header_t *packet = (packet_header_t *)buffer;
packet->mt = 1;  // Payload packet
packet->ttl = ttl;
packet->pl = htonl(payload_size);
```

Sets up the packet header at the start of the buffer:
- `mt = 1`: Identifies this as a regular payload-carrying packet
- `ttl`: Sets the user-provided TTL value
- `pl`: Stores payload length in network byte order using `htonl()` (host-to-network long)
- The sequence number (`sn`) is set later, incrementally for each packet

### Payload Generation

```c
unsigned char *payload = buffer + sizeof(packet_header_t);
for (int i = 0; i < payload_size; i++) {
    payload[i] = rand() % 256;
}
```

Fills the payload area with random data:
- Calculates payload area by offsetting from the start of the buffer by header size
- Generates random bytes (0-255) using the `rand()` function
- Unlike many protocols that carry meaningful data, this implementation uses random data for testing purposes

### Packet Sending Loop

The core loop sends `num_packets` packets sequentially:

```c
for (int i = 0; i < num_packets; i++) {
    packet->sn = htons(i);  // Set sequence number
    
    // Record start time
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);
    
    sendto(sockfd, buffer, sizeof(packet_header_t) + payload_size, 0,
           (struct sockaddr *)&server_addr, sizeof(server_addr));
    
    // ... response handling ...
}
```

For each iteration:
1. Sets the current sequence number in network byte order
2. Records precise timestamp before sending
3. Sends the packet with total size = header + payload
4. The `sendto()` function sends the UDP datagram to the specified server address

### Response Handling

```c
int bytes_received = recvfrom(sockfd, recv_buffer, MAX_BUFFER, 0,
                             (struct sockaddr *)&server_addr, &server_len);
        
gettimeofday(&end_time, NULL);
        
if (bytes_received < 0) {
    printf("Packet %d: Timeout or error\n", i);
    continue;
}
```

After sending, the client:
1. Waits for a response using `recvfrom()` (up to the 2-second timeout)
2. Records the precise end timestamp when a response arrives
3. Handles timeout cases where no response was received

### Error Packet Detection and Handling

```c
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
```

If a response is received, the client:
1. Checks if it's the size of an error packet
2. Verifies it has message type 2 (error)
3. Translates error codes to human-readable messages
4. Skips RTT calculation for error packets since they don't represent successful transmission

### RTT Calculation and Reporting

```c
double rtt = calculate_rtt(start_time, end_time);
printf("Packet %d: RTT = %.3f ms\n", i, rtt);
        
rtt_values[successful_packets++] = rtt;
```

For successful packet exchanges:
1. Calculates round-trip time using the difference between send and receive timestamps
2. Displays the RTT with millisecond precision
3. Stores the RTT value in an array for statistical analysis
4. Increments the counter for successful packets

### Statistical Summary

```c
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
```

After sending all packets:
1. Calculates the sum of all successful RTT values
2. Computes the average RTT (if any successful exchanges)
3. Reports:
   - Total packets attempted
   - Number of successful exchanges
   - Average round-trip time across all successful packets
4. Handles the edge case where no packets were successful

### Cleanup

```c
free(buffer);
close(sockfd);
```

Properly releases resources:
1. Frees the dynamically allocated packet buffer
2. Closes the socket to release system resources

## Running the Client

Compile the program:
```bash
gcc -o ClientPacketGen ClientPacketGen.c
```

Run the client:
```bash
./ClientPacketGen <ServerIP> <ServerPort> <PayloadSize> <TTL> <NumPackets>
```

Example:
```bash
./ClientPacketGen 127.0.0.1 8888 200 10 5
```
This sends 5 packets with 200-byte payloads and TTL=10 to a server running on localhost port 8888.
