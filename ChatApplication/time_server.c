#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<time.h>

#define PORT 4444

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    char buffer[200];
    socklen_t addr_size;
    time_t current_time;
    char time_str[100];

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket < 0) {
        printf("Socket creation failed\n");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if(bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("Binding failed\n");
        exit(1);
    }

    if(listen(serverSocket, 1) == 0) {
        printf("Time Server listening on port %d...\n", PORT);
    }

    while(1) {
        addr_size = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addr_size);
        
        recv(clientSocket, buffer, sizeof(buffer), 0);
        printf("Client Request: %s\n", buffer);

        if(strcmp(buffer, "What's the time?") == 0) {
            time(&current_time);
            strcpy(time_str, ctime(&current_time));
            send(clientSocket, time_str, strlen(time_str), 0);
        }

        close(clientSocket);
    }

    close(serverSocket);
    return 0;
}
