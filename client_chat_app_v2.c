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
    int clientSocket;
    struct sockaddr_in serverAddress;
    char message[200];

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(clientSocket < 0) {
        printf("Socket creation failed\n");
        exit(1);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        printf("Connection failed\n");
        exit(1);
    }

    printf("Connected to server\n");

    while(1) {
        // Get message from user
        printf("Client: ");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = 0;
        
        // Send message to server
        send(clientSocket, message, sizeof(message), 0);
        
        // Check if client wants to end chat
        if(strcmp(message, "Bye") == 0) {
            printf("Chat ended by client\n");
            break;
        }
        
        // Receive response from server
        recv(clientSocket, message, sizeof(message), 0);
        printf("Server: %s\n", message);
        
        // Check if server ended chat
        if(strcmp(message, "Bye") == 0) {
            printf("Chat ended by server\n");
            break;
        }
    }

    close(clientSocket);
    return 0;
}
