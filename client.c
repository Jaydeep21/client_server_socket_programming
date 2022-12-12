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

#define SIZE 1024 //defaulting string size

int main(int argc, char *argv[]){
    char *ip = "127.0.0.1"; //setting ip for serverA
    char *ip1 = "127.0.0.1"; //setting ip for serverB
    int port = htons(8080); //setting port for serverA
    int port2 = htons(8081); //setting port for serverB
    int e; //to store connection op
    int sockfd; //to initialize socketA
    int sockfd2; //to initialize socketB
    struct sockaddr_in server_addr; //to store connection address for server A
    sockfd = socket(AF_INET, SOCK_STREAM, 0); //creating socketA
    if(sockfd < 0) { //incase of any problem while initializing socket
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
    while (1) { //until client exits the server but stating quit or by SIG
        memset(buffer, 0, SIZE); //clear memory for buffer 
        memset(input, 0, SIZE); //clear memory for input string
        printf("chat> ");
        fgets(input, sizeof(input), stdin); //take the input from the terminal
        write(sockfd, input, sizeof(input)); //write the output to server
        if (strncmp(input, "quit", 4) == 0) //if quit is passed as input exit and close socket
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