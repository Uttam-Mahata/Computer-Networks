#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define PORT 4444

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char serverIP[20];
    char buffer[200];
    
    printf("Enter server IP address: ");
    fgets(serverIP, sizeof(serverIP), stdin);
    serverIP[strcspn(serverIP, "\n")] = 0;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(clientSocket < 0) {
        printf("Socket creation failed\n");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);

    if(connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("Connection failed\n");
        exit(1);
    }

    strcpy(buffer, "What's the time?");
    send(clientSocket, buffer, strlen(buffer), 0);
    printf("Requesting time from server...\n");

    recv(clientSocket, buffer, sizeof(buffer), 0);
    printf("Server's time: %s", buffer);

    close(clientSocket);
    return 0;
}
