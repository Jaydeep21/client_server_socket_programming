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
    char *ip1 = "127.0.0.1";
    char *ip = "127.0.0.1";
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
    memset(&server_addr, 0, sizeof(server_addr));  //clear pervious server memory
    //setting up address for serverA
    server_addr.sin_family = AF_INET; 
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    //connect to serverA
    e = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(e == -1) { //if we get error while initializing socketA
        perror("Error in socket");
        exit(1);
    }
	printf("Connected to Server.\n");
    
    int n; //to receive the lenght of input message
    char input[SIZE]; //to get input value
    char buffer[SIZE]; //to get what acknowledgement from server  
    
    n = recv(sockfd, buffer, sizeof(buffer), 0); //receive the acknowledgement from server in buffer variable
    if (strncmp(buffer, "NA", 2) == 0){ //incase of negative ack connect to server B
        close(sockfd); //close previous socket
        sockfd = socket(AF_INET, SOCK_STREAM, 0); //creating socketB
        struct sockaddr_in server_addr2; //to store connection address for server B
        memset(&server_addr2, 0, sizeof(server_addr)); //clear pervious server memory
        //setting up address for serverB
        server_addr2.sin_family = AF_INET; 
        server_addr2.sin_port = port2;
        server_addr2.sin_addr.s_addr = inet_addr(ip1);
        e = connect(sockfd, (struct sockaddr*)&server_addr2, sizeof(server_addr2)); //connect to serverB
        if(e == -1) { //if we get error while initializing socketB
            perror("Error in socket");
            exit(1);
        }
    recv(sockfd, buffer, sizeof(buffer), 0); //receive the message from server in buffer variable
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
        sleep(1); //to make sure that the quit message is read by server 
        recv(sockfd, buffer, sizeof(buffer), 0); //get response from server in normal cases
        printf("%s", buffer); //print the response
    }
    return 1;
}