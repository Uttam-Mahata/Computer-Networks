#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>


// Client Application @ 10.2.1.40
// Server Application @ 10.2.81.238



int main() {

    char sendMessage[200];
    char receiveMessage[200];

    // Predefined Text to be sent to the server
    strcpy(sendMessage, "Hello World!");

    int clientSocket;

    if((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation failed\n");
        exit(1);
    }

    struct sockaddr_in serverAddress;

    int serverPort = 4444;
    serverAddress.sin_family = AF_INET;

    serverAddress.sin_port = htons(serverPort);

    // Internet Address
    serverAddress.sin_addr.s_addr = inet_addr("10.2.81.238");

    if(connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        printf("Connection failed\n");
        exit(1);
    }

    printf("Connected to the server\n");

    send(clientSocket, sendMessage, sizeof(sendMessage), 0);
    printf("Message sent to the server\n");

    recv(clientSocket, receiveMessage, sizeof(receiveMessage), 0);
    printf("Message received from the server: %s\n", receiveMessage);

    close(clientSocket);
    



    return 0;
}
