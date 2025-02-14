#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define MAX 1024

volatile int running = 1;
int sock = 0;

void *send_message(void *arg) {
    char buffer[MAX];
    while(running) {
        printf("Enter message: ");
        fgets(buffer, MAX, stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        
        send(sock, buffer, strlen(buffer), 0);
        
        if (strcmp(buffer, "Bye") == 0) {
            running = 0;
            break;
        }
    }
    return NULL;
}

void *receive_message(void *arg) {
    char buffer[MAX];
    int valread;
    
    while(running) {
        memset(buffer, 0, MAX);
        valread = read(sock, buffer, MAX);
        
        if (valread <= 0 || strcmp(buffer, "Bye") == 0) {
            running = 0;
            break;
        }
        
        printf("\nServer: %s\n", buffer);
    }
    return NULL;
}

int main() {
    struct sockaddr_in serv_addr;
    pthread_t send_thread, receive_thread;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, "192.168.29.22", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    printf("Connected to server. Start chatting!\n");

    pthread_create(&send_thread, NULL, send_message, NULL);
    pthread_create(&receive_thread, NULL, receive_message, NULL);

    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);

    printf("Chat session ended.\n");
    close(sock);
    return 0;
}
