# Socket Programming Examples

This repository contains several examples of socket programming in C. Each example demonstrates a different concept or application.

## Examples

### Chat Application

A simple TCP client-server chat application.

*   **Phase 1:** Basic message exchange with a predefined message.
*   **Phase 2:** Interactive chat between client and server.

See `ChatApplication/Readme.md` for more details.

### Time Server Application

A TCP client-server application that synchronizes time between the client and server.

*   Client sends a request to the server.
*   Server responds with its current time.

See `TimeOfServer/Readme.md` for more details.

### Sorter Application

A TCP server application that sorts a set of integer elements provided by the client.

*   Client sends a list of integers to the server.
*   Server sorts the integers using insertion sort.
*   Server sends the sorted list back to the client.

See `SorterApplication/Readme.md` for more details.

## Problem 1

A basic TCP client-server application for message exchange.

## Problem 2

A time server application similar to the `TimeServer` example, but potentially with different implementation details.

## Problem 3

A sorting server application similar to the `SorterApplication` example, but potentially with different implementation details.

## Usage

Each example has its own directory with a `Readme.md` file containing specific instructions on how to build and run the application.  Generally, you will need to:

1.  Compile the server and client programs using a C compiler (e.g., GCC).
2.  Run the server program, specifying a port number as a command-line argument.
3.  Run the client program, specifying the server's IP address and port number as command-line arguments.

## File Structure

*   `.github/workflows/`: Contains GitHub Actions workflow for building and deploying a Jekyll site to GitHub Pages.
*   `.vscode/`: Contains VS Code settings.
*   `ChatApplication/`: Contains the chat application source code and documentation.
*   `Problem-1/`: Contains the source code for Problem 1.
*   `Problem-2/`: Contains the source code for Problem 2.
*   `Problem-3/`: Contains the source code for Problem 3.
*   `SorterApplication/`: Contains the sorter application source code and documentation.
*   `TimeOfServer/`: Contains the time server application source code and documentation.
*   `Readme.md`: This file.
