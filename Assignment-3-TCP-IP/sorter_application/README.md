# Network Sorting Application

A client-server application demonstrating binary protocols and network byte ordering.

## Binary Protocol Concepts

### Network Byte Order Functions

#### htons() / ntohs()
```c
uint16_t htons(uint16_t hostshort);  // Host to network (short)
uint16_t ntohs(uint16_t netshort);   // Network to host (short)
```
- Converts 16-bit integers between host and network byte order
- Used for port numbers and element counts
- Essential for cross-platform compatibility

#### htonl() / ntohl()
```c
uint32_t htonl(uint32_t hostlong);   // Host to network (long)
uint32_t ntohl(uint32_t netlong);    // Network to host (long)
```
- Converts 32-bit integers between host and network byte order
- Used for array elements
- Ensures consistent data interpretation

### Binary Packet Structure

#### Header Format
```
[2 bytes: Number of elements (uint16_t)]
```
- Fixed-size header
- Network byte order
- Maximum 1000 elements

#### Data Format
```
[4 bytes: Element 1 (int32_t)]
[4 bytes: Element 2 (int32_t)]
...
[4 bytes: Element N (int32_t)]
```
- Fixed-size elements
- Network byte order
- Contiguous memory layout

## Sorting Algorithm Implementation

### Insertion Sort
```c
void insertionSort(int arr[], int n) {
    int i, key, j;
    for (i = 1; i < n; i++) {
        key = arr[i];
        j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}
```
- In-place sorting
- O(n²) time complexity
- Stable sorting algorithm

## Binary Protocol Implementation

### Packet Assembly
```c
// Write element count
uint16_t network_count = htons(num_elements);
memcpy(packet, &network_count, 2);

// Write elements
for (int i = 0; i < num_elements; i++) {
    int32_t network_value = htonl(elements[i]);
    memcpy(packet + 2 + (i * 4), &network_value, 4);
}
```

### Packet Parsing
```c
// Read element count
uint16_t network_count;
memcpy(&network_count, packet, 2);
num_elements = ntohs(network_count);

// Read elements
for (int i = 0; i < num_elements; i++) {
    int32_t network_value;
    memcpy(&network_value, packet + 2 + (i * 4), 4);
    elements[i] = ntohl(network_value);
}
```

## Memory Management

### Buffer Sizing
```c
#define MAX_ELEMENTS 1000
#define MAX_PACKET_SIZE (2 + (MAX_ELEMENTS * 4))
```
- Static allocation
- Size validation
- Overflow prevention

### Memory Layout
```
[Header (2 bytes)][Element 1 (4 bytes)][Element 2 (4 bytes)]...
```
- Aligned access
- No padding
- Sequential layout

## Error Handling

### Size Validation
```c
if (num_elements > MAX_ELEMENTS) {
    fprintf(stderr, "Too many elements\n");
    exit(EXIT_FAILURE);
}
```

### Network Errors
```c
if (bytes_read != expected_size) {
    perror("Incomplete read");
    exit(EXIT_FAILURE);
}
```

## Protocol Flow Diagram
```
Client                              Server
  |                                   |
  |----[Header + Unsorted Array]----->|
  |                                   |
  |                          [Sort Array]
  |                                   |
  |<---[Header + Sorted Array]-------|
```

## Features

- Binary protocol implementation
- Network byte order handling
- Insertion sort algorithm
- Fixed-size packets
- Efficient memory usage

## Technical Requirements

- C99 or later
- Network byte order support
- TCP socket support
- Buffer management

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

4. Usage Instructions:
   - Enter number of elements (1-1000)
   - Input integers one per line
   - View sorted result

## Implementation Notes

### Performance Considerations
- Efficient binary protocol
- Minimal memory copying
- Network byte order conversion cost
- Sorting complexity analysis

### Error Cases
- Buffer overflow protection
- Network transmission errors
- Invalid element counts
- Memory allocation failures