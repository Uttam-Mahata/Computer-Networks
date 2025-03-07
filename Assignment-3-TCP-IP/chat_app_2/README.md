# Interactive Chat Application (Version 2)

A bidirectional chat application demonstrating interactive socket communication.

## Advanced Socket Programming Concepts

### Buffer Management
```c
#define MAX_BUFFER 1024
char buffer[MAX_BUFFER] = {0};
```
- Fixed-size buffer for message handling
- Zero initialization prevents data leakage
- Buffer overflow protection

### String Handling Functions

#### memset()
```c
void *memset(void *s, int c, size_t n)
```
- Clears buffer before each receive operation
- Prevents cross-message contamination
- Used as: `memset(buffer, 0, MAX_BUFFER);`

#### strcspn()
```c
size_t strcspn(const char *s, const char *reject)
```
- Removes newline from user input
- Used as: `buffer[strcspn(buffer, "\n")] = 0;`

### Interactive Communication Flow

#### Message Loop
```c
while (1) {
    // Receive message
    // Process message
    // Send response
    // Check for termination
}
```
- Continuous message exchange
- Termination condition checking
- Alternating read/write operations

### Socket Options

#### Socket Timeouts
```c
struct timeval timeout;
setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
```
- Optional: Can be used to prevent blocking indefinitely
- Allows for graceful termination
- Enables responsive user interaction

### Message Protocol

#### Message Format
- Raw text messages
- Null-terminated strings
- "Bye" as termination signal
- Length limited by MAX_BUFFER

#### Flow Control
```
Client → Server: Message
Server → Client: Response
Client → Server: Next message
...
Either → Other: "Bye"
```

## Implementation Details

### Server Implementation
1. Socket Setup Phase:
   ```c
   socket() → bind() → listen() → accept()
   ```

2. Communication Phase:
   ```c
   while (1) {
       recv() - Get client message
       process message
       get user input
       send() - Send response
       check for "Bye"
   }
   ```

3. Cleanup Phase:
   ```c
   close() - Close both sockets
   ```

### Client Implementation
1. Connection Phase:
   ```c
   socket() → connect()
   ```

2. Communication Phase:
   ```c
   while (1) {
       get user input
       send() - Send message
       recv() - Get response
       check for "Bye"
   }
   ```

3. Cleanup Phase:
   ```c
   close() - Close socket
   ```

## Error Handling Mechanisms

### Connection Errors
```c
if (connect(sock_fd, ...) < 0) {
    perror("Connection failed");
    exit(1);
}
```
- Immediate error reporting
- Clean program termination
- Resource cleanup

### Communication Errors
```c
if (bytes_received <= 0) {
    // Handle disconnection or error
    break;
}
```
- Detect connection loss
- Handle partial receives
- Manage orderly shutdowns

## Network Protocol State Machine
```
          [INIT]
             ↓
    [WAIT_FOR_MESSAGE]
        ↙           ↘
[PROCESS_MESSAGE]  [CHECK_BYE]
        ↘           ↙
     [SEND_REPLY]
          ↓
    [CHECK_STATUS]
     ↙          ↘
[CONTINUE]    [TERMINATE]
```

## Features

- Two-way interactive communication
- Real-time message exchange
- Support for chat termination ("Bye" message)
- Improved error handling
- Session persistence until explicit termination

## Technical Details

- TCP socket-based communication
- Command-line port configuration
- Written in C
- Supports IPv4 addressing
- Blocking I/O for message handling

## How to Use

1. Compile the programs:
```bash
gcc server.c -o server
gcc client.c -o client
```

2. Start the server with a port number:
```bash
./server <port>
```

3. Start the client:
```bash
./client <server_ip> <port>
```

4. Chat Instructions:
   - Type messages and press Enter to send
   - Type "Bye" to end the chat session
   - Messages are displayed in real-time
   - Server and client take turns sending messages