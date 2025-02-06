# Time Server Application

A simple TCP Client-Server application demonstrating time synchronization.

## Description
This application consists of a client and server component where:
- Client sends a "What's the time?" message to the server
- Server responds with its current time
- Client displays the received time

## Protocol
1. Client connects to server on user-defined port
2. Client sends: "What's the time?"
3. Server responds: Current server time
4. Client displays received time

## Setup
- Server runs on a configurable port
- Client connects to the same port
- Uses TCP protocol for reliable communication

## Usage
1. Start the server application
2. Run the client application
3. View the synchronized time