#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define PORT 4444

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;
    char message[200];
    socklen_t addr_size;

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket < 0) {
        printf("Socket creation failed\n");
        exit(1);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind
    if(bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        printf("Binding failed\n");
        exit(1);
    }

    // Listen
    if(listen(serverSocket, 1) == 0) {
        printf("Listening...\n");
    } else {
        printf("Listen failed\n");
        exit(1);
    }

    addr_size = sizeof(clientAddress);
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &addr_size);

    // Receive message
    recv(clientSocket, message, sizeof(message), 0);
    printf("Message from client: %s\n", message);

    // Send back the same message
    send(clientSocket, message, sizeof(message), 0);
    printf("Message sent back to client\n");

    close(clientSocket);
    close(serverSocket);
    return 0;
}







