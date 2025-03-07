# Basic Chat Application (Version 1)

A simple client-server chat application demonstrating basic socket programming concepts.

## Socket Programming Concepts Used

### Socket Creation
```c
int socket(int domain, int type, int protocol)
```
- **domain**: AF_INET for IPv4 Internet protocols
- **type**: SOCK_STREAM for TCP connection
- **protocol**: 0 for default protocol (TCP)
- **Returns**: File descriptor for the new socket

### Address Structure
```c
struct sockaddr_in {
    sa_family_t sin_family;     // Address family (AF_INET)
    in_port_t sin_port;         // Port number in network byte order
    struct in_addr sin_addr;     // IPv4 address
};
```
- Used for configuring network address and port
- Requires network byte order conversion using `htons()` for port

### Server-Specific Functions

#### bind()
```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
```
- Associates socket with specific address and port
- Required for server to listen on specific port
- Returns 0 on success, -1 on error

#### listen()
```c
int listen(int sockfd, int backlog)
```
- Marks socket as passive socket for accepting connections
- backlog: Maximum length of pending connections queue
- Returns 0 on success, -1 on error

#### accept()
```c
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
```
- Accepts incoming connection request
- Creates new socket for the connection
- Returns new socket file descriptor

### Client-Specific Functions

#### connect()
```c
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
```
- Initiates connection to server
- Requires server's address and port
- Returns 0 on success, -1 on error

### Common Communication Functions

#### send()
```c
ssize_t send(int sockfd, const void *buf, size_t len, int flags)
```
- Transmits data to connected socket
- Returns number of bytes sent
- May send fewer bytes than requested

#### recv()
```c
ssize_t recv(int sockfd, void *buf, size_t len, int flags)
```
- Receives data from connected socket
- Returns number of bytes received
- Returns 0 for orderly shutdown, -1 for error

### Error Handling
```c
void perror(const char *s)
```
- Prints descriptive error message
- Uses errno to determine specific error

## Implementation Flow

### Server Side
1. Create socket using `socket()`
2. Configure server address structure
3. Bind socket to address using `bind()`
4. Listen for connections with `listen()`
5. Accept client connection via `accept()`
6. Receive message using `recv()`
7. Echo message back using `send()`
8. Close connection

### Client Side
1. Create socket using `socket()`
2. Configure server address structure
3. Connect to server using `connect()`
4. Send message using `send()`
5. Receive echo using `recv()`
6. Close connection

## Socket States and Flow
```
Server                      Client
socket()                    socket()
   ↓                           ↓
bind()                     connect()
   ↓                           ↓
listen()                   send()
   ↓                           ↓
accept()                   recv()
   ↓                           ↓
recv()                     close()
   ↓
send()
   ↓
close()
```

## Error Handling Examples
- Socket creation failure
- Bind failure (address already in use)
- Connection failures
- Send/Receive errors
- Resource cleanup

## Features

- One-way message communication
- Single client support
- Simple echo functionality
- Basic error handling

## Technical Details

- TCP socket-based communication
- Command-line port configuration
- Written in C
- Supports IPv4 addressing

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

## Implementation Details

### Server
- Creates a TCP socket and listens for connections
- Accepts a single client connection
- Echoes back received messages
- Includes detailed error handling and socket cleanup

### Client
- Connects to server using IP and port
- Sends a predefined "Hello World!!" message
- Displays the server's echo response
- Automatically closes connection after message exchange