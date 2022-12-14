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

// Use Colours for bash text
#define SET_COLOUR    "\x1b[33m"
#define RESET_COLOR   "\x1b[0m"


int main(int argc, char *argv[]){
    // Change ip if running on two different devices and replace with device ip
    char *ip1 = "192.168.2.124";
    char *ip = "192.168.2.171";
    // Server A running on port 8080 and server B on port 8081
    int port = htons(8080);
    int port2 = htons(8081);
    int e;
    // Define descriptor for socket
    int sockfd;
    struct sockaddr_in server_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        perror("Error in socket");
        exit(1);
    }
    memset(&server_addr, 0, sizeof(server_addr)); // Clear value of variable
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // Connect to server
    e = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(e == -1) {
        perror("Error in socket");
        exit(1);
    }
	printf("Connected to Server.\n");
    
    int n;
    char input[SIZE];
    char buffer[SIZE];
    
    // Receive acknowledgement
    n = recv(sockfd, buffer, sizeof(buffer), 0);

    // If acknowledgement is NA, close connection and connect with server B
    if (strncmp(buffer, "NA", 2) == 0){
        close(sockfd);
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in server_addr2;
        memset(&server_addr2, 0, sizeof(server_addr)); 
        server_addr2.sin_family = AF_INET;
        server_addr2.sin_port = port2;
        server_addr2.sin_addr.s_addr = inet_addr(ip1);
        e = connect(sockfd, (struct sockaddr*)&server_addr2, sizeof(server_addr2));
        if(e == -1) {
            perror("Error in socket");
            exit(1);
        }
    recv(sockfd, buffer, sizeof(buffer), 0);
    }
    while (1) {

        memset(buffer, 0, SIZE); // Clear value of variable
        memset(input, 0, SIZE);
        printf(SET_COLOUR "command$ " RESET_COLOR);
        fgets(input, sizeof(input), stdin); // Get input
        write(sockfd, input, sizeof(input)); // Write to sserver
        if (strncmp(input, "quit", 4) == 0) // close connection and exit if quit is entered
        {
            close(sockfd);
            exit(0);
        }
        sleep(1);
        recv(sockfd, buffer, sizeof(buffer), 0); // Recv from server
        printf("%s", buffer); // Display what is received
    }
    return 1;
}