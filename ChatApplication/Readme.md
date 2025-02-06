# TCP Chat Application

## Problem Description

Develop a basic TCP Client-Server chat application with the following features:

### Phase 1: Basic Message Exchange
- Client sends predefined "Hello World!!" message to server
- Server runs on user-defined port
- Server forwards received message back to client
- Both applications print the messages

### Phase 2: Interactive Chat
- Client initiates conversation with first message
- Server prints received message and sends response
- Alternating message exchange continues
- Chat ends when either party sends "Bye"
- Both parties close session after "Bye" message

## Requirements
- Implementation in C programming language
- Sequential message exchange (wait for response before sending next message)
- TCP protocol for reliable communication

## Constraints
- One message at a time per party
- Must wait for response before sending next message