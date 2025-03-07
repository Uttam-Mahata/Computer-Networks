# Socket Programming Examples in C

A comprehensive collection of network programming examples demonstrating various aspects of socket programming and network protocols.

## Core Socket Programming Concepts

### Socket Types and Domains

#### Stream Sockets (SOCK_STREAM)
- TCP/IP based
- Connection-oriented
- Reliable data delivery
- Used in all chat applications

#### Internet Protocol Family (AF_INET)
- IPv4 addressing
- Port-based services
- Network byte ordering
- Host-to-network conversions

### Common Socket Functions

#### Basic Socket Operations
```c
socket()   // Create new socket
bind()     // Associate socket with address
listen()   // Mark socket as passive
accept()   // Accept incoming connection
connect()  // Connect to remote host
```

#### Data Transfer
```c
send()     // Send data on connected socket
recv()     // Receive data from socket
close()    // Close socket connection
```

### Network Address Structures
```c
struct sockaddr_in {
    sa_family_t sin_family;     // AF_INET
    in_port_t sin_port;         // Port number
    struct in_addr sin_addr;    // IPv4 address
};
```

## Application Categories

### Progressive Learning Path

1. **Basic Communication** (chat_app_1)
   - Single message exchange
   - Basic socket setup
   - Simple error handling

2. **Interactive Communication** (chat_app_2)
   - Two-way messaging
   - Session management
   - Enhanced error handling

3. **Concurrent Communication** (chat_app_3)
   - Multi-threaded design
   - Real-time messaging
   - Thread synchronization

### Specialized Applications

1. **Time Services** (time_of_server)
   - Time synchronization
   - Formatted time handling
   - Simple request-response

2. **Data Processing** (sorter_application)
   - Binary protocols
   - Network byte ordering
   - Array manipulation

3. **Multi-Service** (ask_server)
   - Menu-driven interface
   - Multiple services
   - State management

## Advanced Topics Covered

### Threading Concepts
- POSIX threads (pthread)
- Thread synchronization
- Shared resource management
- Race condition prevention

### Protocol Design
- Text-based protocols
- Binary protocols
- Request-response patterns
- Message formatting

### Network Programming Patterns
- Iterative servers
- Concurrent servers
- Connection management
- Resource cleanup

### Error Handling
- Socket errors
- Network failures
- Resource cleanup
- User input validation

## Additional Resources

### [Exponential Backoff](exponential_backoff.md)
- Network congestion handling
- Retry mechanisms
- Collision avoidance
- Mathematical analysis

## Building and Running

Each application includes:
- Detailed README
- Compilation instructions
- Usage examples
- Error handling guides

### General Compilation Pattern
```bash
# Basic applications
gcc server.c -o server
gcc client.c -o client

# Threaded applications
gcc server.c -o server -pthread
gcc client.c -o client -pthread
```

## Learning Path Recommendations

1. Start with chat_app_1 for basic concepts
2. Progress to chat_app_2 for interactive features
3. Explore time_of_server for simple services
4. Study sorter_application for binary protocols
5. Implement chat_app_3 for threading concepts
6. Master ask_server for complete service design

## Best Practices

### Code Organization
- Modular design
- Clear error handling
- Proper resource management
- Consistent style

### Network Programming
- Buffer management
- Error checking
- Proper cleanup
- Protocol documentation

### Thread Safety
- Synchronization
- Resource protection
- State management
- Clean termination