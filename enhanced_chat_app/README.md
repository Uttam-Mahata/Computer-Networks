# Multithreaded Chat Application (Version 3)

A concurrent chat application using POSIX threads for simultaneous message handling.

## Multithreading Concepts

### POSIX Thread Functions

#### pthread_create()
```c
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                  void *(*start_routine) (void *), void *arg)
```
- Creates new thread
- Assigns thread ID to `thread`
- `start_routine`: Function to execute in new thread
- `arg`: Arguments passed to start_routine

#### pthread_join()
```c
int pthread_join(pthread_t thread, void **retval)
```
- Waits for thread termination
- Collects thread's return value
- Essential for clean thread cleanup

### Thread Synchronization

#### Volatile Variables
```c
volatile int running = 1;
```
- Prevents compiler optimization
- Ensures visibility across threads
- Used for thread termination signaling

### Thread Communication Pattern

#### Message Sending Thread
```c
void *send_message(void *arg) {
    while(running) {
        get user input
        send message
        check termination
    }
    return NULL;
}
```

#### Message Receiving Thread
```c
void *receive_message(void *arg) {
    while(running) {
        receive message
        display message
        check termination
    }
    return NULL;
}
```

## Concurrent Socket Programming

### Thread-Safe Socket Operations

#### Global Socket Variables
```c
int client_socket;  // Shared between threads
```
- Must be properly synchronized
- Access should be atomic
- Lifecycle managed by main thread

### Socket States in Threading Context

#### Main Thread
```
socket() → connect()/accept() → create_threads() → join_threads() → close()
```

#### Sender Thread
```
while(running) {
    send() → check_termination
}
```

#### Receiver Thread
```
while(running) {
    recv() → check_termination
}
```

## Thread Safety Considerations

### Race Conditions Prevention
- Use atomic operations
- Implement proper synchronization
- Avoid shared resource conflicts

### Resource Management
```c
pthread_t send_thread, receive_thread;
pthread_create(&send_thread, NULL, send_message, NULL);
pthread_create(&receive_thread, NULL, receive_message, NULL);
```
- Thread creation error handling
- Resource cleanup in all threads
- Proper thread termination sequence

## Error Handling in Threaded Context

### Thread Creation Errors
```c
if (pthread_create(&thread, NULL, function, arg) != 0) {
    perror("Thread creation failed");
    exit(EXIT_FAILURE);
}
```

### Socket Errors in Threads
```c
if (send(sock_fd, message, len, 0) < 0) {
    running = 0;  // Signal all threads to terminate
    perror("Send failed");
    pthread_exit(NULL);
}
```

## Thread Synchronization Patterns

### Producer-Consumer Pattern
```
[User Input Thread] → [Socket Send Thread]
[Socket Receive Thread] → [Display Thread]
```

### Thread Termination Flow
```
          [Main Thread]
           ↙          ↘
[Send Thread]    [Receive Thread]
           ↘          ↙
        [Cleanup Phase]
```

## Features

- Concurrent message sending and receiving
- No blocking on user input or network
- Real-time bidirectional communication
- Graceful termination handling
- Thread-safe operations

## Technical Requirements

- POSIX threads library (pthread)
- TCP socket support
- C11 or later for atomic operations
- Thread-safe standard library functions

## How to Use

1. Compile with pthread support:
```bash
gcc server.c -o server -pthread
gcc client.c -o client -pthread
```

2. Start the server:
```bash
./server
```

3. Start the client:
```bash
./client
```

## Implementation Notes

### Thread Management
- Main thread handles connection setup
- Separate threads for send/receive
- Graceful shutdown on 'Bye' message
- Proper thread cleanup sequence

### Network Communication
- Non-blocking message handling
- Real-time message display
- Thread-safe socket operations
- Concurrent data transfer