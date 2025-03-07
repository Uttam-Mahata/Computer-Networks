# Multi-Service Server Application

A server application demonstrating multiple service handling through a menu-driven interface.

## Protocol Design

### Query Types
```c
enum QueryType {
    TIME_ONLY = 1,      // Current time (HH:MM:SS)
    DATE_TIME = 2,      // Full date and time
    SERVER_NAME = 3     // System hostname
};
```

### Message Format

#### Request
```
[1 byte: Query Type (1-3)]
```

#### Response
```
[Variable bytes: ASCII Response]
- Time: "HH:MM:SS"
- Date/Time: "YYYY-MM-DD HH:MM:SS"
- Hostname: "hostname"
```

## Service Implementation

### Time Service Handler
```c
case TIME_ONLY:
    strftime(response, BUFFER_SIZE, "%H:%M:%S", time_info);
```
- Provides current time
- 24-hour format
- Second precision

### Date-Time Service Handler
```c
case DATE_TIME:
    strftime(response, BUFFER_SIZE, "%Y-%m-%d %H:%M:%S", time_info);
```
- Full date and time
- ISO 8601-like format
- Calendar date included

### Hostname Service Handler
```c
case SERVER_NAME:
    gethostname(response, BUFFER_SIZE);
```
- System hostname
- POSIX compliant
- Buffer-safe implementation

## Menu System Design

### Client-Side Menu
```c
void print_menu() {
    printf("\nAvailable queries:\n");
    printf("1: Get server time\n");
    printf("2: Get server date and time\n");
    printf("3: Get server name\n");
    printf("0: Exit\n");
}
```
- Clear user interface
- Numbered options
- Exit functionality

### Input Validation
```c
if (choice < 0 || choice > 3) {
    printf("Invalid option\n");
    continue;
}
```
- Range checking
- Error recovery
- User feedback

## State Management

### Connection States
```
[INIT] → [MENU] → [SEND_QUERY] → [RECEIVE_RESPONSE] → [DISPLAY] → [MENU/EXIT]
```

### Error States
```
[ERROR_INVALID_CHOICE] → [MENU]
[ERROR_CONNECTION] → [EXIT]
[ERROR_RESPONSE] → [MENU]
```

## Implementation Details

### Server Architecture
```c
while(1) {
    accept_connection();
    read_query_type();
    process_request();
    send_response();
    cleanup_connection();
}
```
- Iterative server design
- Single request per connection
- Clean connection handling

### Client Architecture
```c
while(1) {
    display_menu();
    get_user_choice();
    if (choice == 0) break;
    
    connect_to_server();
    send_request();
    display_response();
    close_connection();
}
```
- Interactive menu loop
- Connection per request
- Graceful exit handling

## Error Handling

### Menu Validation
```c
while(getchar() != '\n');  // Clear input buffer
if (scanf("%d", &choice) != 1) {
    printf("Invalid input\n");
    continue;
}
```
- Buffer overflow prevention
- Type checking
- Input recovery

### Network Error Handling
```c
if (connect(sock_fd, ...) < 0) {
    perror("Connection failed");
    return handle_error();
}
```
- Connection failures
- Transmission errors
- Resource cleanup

## Features

- Multiple service types
- User-friendly menu interface
- Robust error handling
- Clean protocol design

## Technical Requirements

- TCP socket support
- POSIX system calls
- Standard C library

## How to Use

1. Compile the programs:
```bash
gcc server.c -o server
gcc client.c -o client
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

### Service Extensibility
- Easy to add new services
- Protocol supports expansion
- Modular design

### Connection Management
- One request per connection
- Clean resource handling
- Proper error recovery