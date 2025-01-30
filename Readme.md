# TCP Chat & Time Server Applications

A collection of TCP-based applications implemented in C:
- Version 1: Simple message exchange
- Version 2: Interactive chat session
- Time Server: Server time request/response

## Prerequisites

- GCC compiler
- Linux/Unix environment
- Basic understanding of TCP/IP networking

## Project Structure

```
.
├── chat_app_v1.c        # Server application (Version 1)
├── client_chat_app_v1.c # Client application (Version 1)
├── chat_app_v2.c        # Server application (Version 2)
├── client_chat_app_v2.c # Client application (Version 2)
├── time_server.c        # Time server application
└── time_client.c        # Time client application
```

## Version 1: Simple Message Exchange

This version demonstrates basic TCP communication where:
- Client sends "Hello World!" to server
- Server receives and displays the message
- Server sends back the same message
- Client receives and displays the message

### Compilation
```bash
gcc chat_app_v1.c -o server_v1
gcc client_chat_app_v1.c -o client_v1
```

### Running
1. Start server:
```bash
./server_v1
```
2. In another terminal, start client:
```bash
./client_v1
```

## Version 2: Interactive Chat

This version implements a full chat application where:
- Client and server can exchange messages alternatively
- Each party must wait for the other's response before sending next message
- Chat session ends when either party types "Bye"

### Compilation
```bash
gcc chat_app_v2.c -o server_v2
gcc client_chat_app_v2.c -o client_v2
```

### Running
1. Start server:
```bash
./server_v2
```
2. In another terminal, start client:
```bash
./client_v2
```

## Time Server Application

This version implements a simple time server where:
- Client sends "What's the time?" message
- Server responds with its current time
- Client displays the received time

### Compilation
```bash
gcc time_server.c -o time_server
gcc time_client.c -o time_client
```

### Running
1. Start time server:
```bash
./time_server
```
2. In another terminal, start time client:
```bash
./time_client
```
3. Enter the server's IP address when prompted

### Protocol
1. Connection establishment
2. Client sends "What's the time?" message
3. Server responds with current time
4. Connection termination

## Configuration

### Default Settings
- Port: 4444
- Server IP: Currently set to "10.2.81.238" (modify in client code)
- Maximum message length: 200 characters

### Modifying Server IP
Change the IP address in client applications:
```c
serverAddress.sin_addr.s_addr = inet_addr("YOUR_SERVER_IP");
```

## Protocol

1. Connection establishment
2. Message exchange:
   - Client sends message
   - Server receives and displays
   - Server sends response
   - Client receives and displays
3. Session termination:
   - Either party sends "Bye"
   - Both applications close the connection

## Error Handling

The application handles basic errors including:
- Socket creation failure
- Connection failure
- Binding failure
- Listen failure

## Notes

- Always start the server before the client
- Version 2 requires alternating messages between client and server
- Type "Bye" to end the chat session
- Time server automatically closes connection after sending time
- Make sure to use correct server IP address in client applications