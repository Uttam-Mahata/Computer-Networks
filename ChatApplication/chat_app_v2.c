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

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket < 0) {
        printf("Socket creation failed\n");
        exit(1);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if(bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        printf("Binding failed\n");
        exit(1);
    }

    if(listen(serverSocket, 1) == 0) {
        printf("Listening...\n");
    } else {
        printf("Listen failed\n");
        exit(1);
    }

    addr_size = sizeof(clientAddress);
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &addr_size);
    
    while(1) {
        // Receive message from client
        recv(clientSocket, message, sizeof(message), 0);
        printf("Client: %s\n", message);
        
        // Check for "Bye" message
        if(strcmp(message, "Bye") == 0) {
            printf("Chat ended by client\n");
            break;
        }
        
        // Get server's response
        printf("Server: ");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = 0;
        
        // Send message to client
        send(clientSocket, message, sizeof(message), 0);
        
        // Check if server wants to end chat
        if(strcmp(message, "Bye") == 0) {
            printf("Chat ended by server\n");
            break;
        }
    }

    close(clientSocket);
    close(serverSocket);
    return 0;
}
