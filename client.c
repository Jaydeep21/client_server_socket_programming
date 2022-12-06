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
    // char *ip = "127.0.0.1";
    int port = htons(8080);
    int port2 = htons(8081);
    int e;
    int sockfd;
    int sockfd2;
    struct sockaddr_in server_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        perror("Error in socket");
        exit(1);
    }
    // printf("Server socket created successfully.\n");
    memset(&server_addr, 0, sizeof(server_addr)); 
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = INADDR_ANY;

    e = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(e == -1) {
        perror("Error in socket");
        exit(1);
    }
	printf("Connected to Server.\n");
    
    int n;
    char input[SIZE];
    char buffer[SIZE];
    
    n = recv(sockfd, buffer, sizeof(buffer), 0);
    // printf("%s",buffer);
    if (strncmp(buffer, "No", 2) == 0){
        close(sockfd);
        // printf("Connection Closed with Server A");
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in server_addr2;
        memset(&server_addr2, 0, sizeof(server_addr)); 
        server_addr2.sin_family = AF_INET;
        server_addr2.sin_port = port2;
        server_addr2.sin_addr.s_addr = INADDR_ANY;
        e = connect(sockfd, (struct sockaddr*)&server_addr2, sizeof(server_addr2));
        if(e == -1) {
            perror("Error in socket");
            exit(1);
        }
    recv(sockfd, buffer, sizeof(buffer), 0);
    }
    while (1) {
        memset(buffer, 0, SIZE);
        memset(input, 0, SIZE);
        
        printf("chat> ");
        fgets(input, sizeof(input), stdin);
        write(sockfd, input, sizeof(input));
        if (strncmp(input, "quit", 4) == 0)
        {
            close(sockfd);
            exit(0);
        }
        sleep(1);
        recv(sockfd, buffer, sizeof(buffer), 0);
        printf("%s", buffer);
    }
    return 1;
}