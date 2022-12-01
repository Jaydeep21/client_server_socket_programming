#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SIZE 1024

int main(int argc, char *argv[]){
    char message[1000];
    char *ip = "127.0.0.1";
    int port = 8080;
    int e;
    int sockfd;
    struct sockaddr_in server_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        perror("Error in socket");
        exit(1);
    }
    printf("Server socket created successfully.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    e = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(e == -1) {
        perror("Error in socket");
        exit(1);
    }
	printf("Connected to Server.\n");
    
    /* Receive final data from server and display on screen */
    int n;
    char input[SIZE];
    char buffer[SIZE];

    // dup2(sockfd, 1);
    while (1) {
        printf("chat> ");
        gets(input);
        send(sockfd, input, sizeof(input), 0);
        if (strcmp(input, "quit") == 0)
        {
            close(sockfd);
            exit(0);
        }
        n = recv(sockfd, buffer, SIZE, 0);
        printf("Rec size: %d\n",n);
        printf(buffer);
        bzero(buffer, SIZE);
        bzero(input, SIZE);
    }
    return 1;
}