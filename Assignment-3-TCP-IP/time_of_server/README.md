# Time Server Application

A network time synchronization application demonstrating time-based socket programming.

## Time Programming Concepts

### Time Functions

#### time()
```c
time_t time(time_t *tloc)
```
- Returns current calendar time
- Number of seconds since Epoch
- Thread-safe time acquisition

#### localtime()
```c
struct tm *localtime(const time_t *timep)
```
- Converts time_t to broken-down time
- Handles timezone conversion
- Returns pointer to static data

#### strftime()
```c
size_t strftime(char *s, size_t max, const char *format,
                const struct tm *tm)
```
- Formats time according to format string
- Thread-safe string generation
- Handles locale-specific formatting

### Time Data Structures

#### struct tm
```c
struct tm {
    int tm_sec;    /* Seconds (0-60) */
    int tm_min;    /* Minutes (0-59) */
    int tm_hour;   /* Hours (0-23) */
    int tm_mday;   /* Day of the month (1-31) */
    int tm_mon;    /* Month (0-11) */
    int tm_year;   /* Year - 1900 */
    int tm_wday;   /* Day of the week (0-6, Sunday = 0) */
    int tm_yday;   /* Day in the year (0-365) */
    int tm_isdst;  /* Daylight saving time */
};
```

## Network Time Protocol Concepts

### Time Synchronization Flow
```
[Client] ----Request----> [Server]
         <---Time Data----
```

### Time Format
```c
strftime(time_str, MAX_BUFFER, "%Y-%m-%d %H:%M:%S", localtime(&now));
```
- ISO 8601-like format
- Human-readable representation
- Includes date and time

## Socket Programming Implementation

### Server-Side Time Handling
```c
void handle_time_request(int client_fd) {
    time_t now = time(NULL);
    char time_str[MAX_BUFFER];
    strftime(time_str, MAX_BUFFER, "%Y-%m-%d %H:%M:%S", 
             localtime(&now));
    send(client_fd, time_str, strlen(time_str), 0);
}
```

### Client-Side Time Reception
```c
void receive_time(int sock_fd) {
    char buffer[MAX_BUFFER];
    recv(sock_fd, buffer, MAX_BUFFER, 0);
    printf("Server time: %s\n", buffer);
}
```

## Error Handling Specifics

### Time Function Errors
```c
if (time(&now) == -1) {
    perror("Time acquisition failed");
    return;
}
```

### Format Errors
```c
if (strftime(...) == 0) {
    fprintf(stderr, "Time formatting failed\n");
    return;
}
```

## Implementation Flow

### Server Implementation
1. Time Request Reception:
   ```
   accept() → recv() → get_time() → format_time() → send()
   ```

2. Error Cases:
   - Time acquisition failure
   - Format buffer overflow
   - Network transmission errors

### Client Implementation
1. Time Request Process:
   ```
   connect() → send_request() → recv_time() → display()
   ```

2. Error Handling:
   - Connection timeout
   - Invalid time format
   - Network errors

## Protocol Design

### Request Format
- Simple text-based request
- No specific payload required
- Connection itself signals request

### Response Format
- Text-based time string
- ISO 8601-like format
- Null-terminated string

## Features

- Current time retrieval
- Timezone-aware time formatting
- Multiple client support
- Clean error handling

## Technical Details

- TCP socket-based communication
- System time functions
- Format string customization
- Configurable port number

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

## Implementation Notes

### Time Precision
- Second-level precision
- Local timezone handling
- Consistent formatting

### Network Considerations
- Minimal network overhead
- Single request-response cycle
- Connection cleanup after use