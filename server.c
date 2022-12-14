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


// Function that handles all clients
int serviceClient(int new_sock){
    char opt[SIZE];
    int saved_stdout = dup(1); // Save current value of standard input
    dup2(new_sock, 0); // Change everything to socked fd
    dup2(new_sock, 1);
    dup2(new_sock, 2);
    char word[SIZE];
    while(1){
        bzero(word, SIZE);
        bzero(opt, SIZE);
        if (read(0, word, sizeof(word))<0){
            printf("read() error\n");
            exit(3); 
        }
        // If quit encountered, close connection and exit
        if (strncmp(word, "quit", 4) == 0)
        {
            dup2(saved_stdout, 1); // Restore original standard input to print below message
            printf("Client Disconnected\n\n");
            close(new_sock); 
            exit(0);
        }
        printf("\n");
        // Execute the command via system, output will be sent to sock fd
        system(word);
    }
}

int main(){
    // Change ip if running on two different devices and replace with device ip
    char *ip = "192.168.2.124";
    char *ip1 = "192.168.2.171";
    // Server A running on port 8080 and server B on port 8081
    int portA = htons(8080);
    int portB = htons(8081);
    int eA, eB, count = 0;
    int flag = 0; // To determine if it's server A or server B, 0 for A, 1 for B
    int sockfdA, sockfdB, new_sock;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;

    sockfdA = socket(AF_INET, SOCK_STREAM, 0);
    sockfdB = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfdA < 0) {
        perror("Error in socketA");
        exit(1);
    }
    if(sockfdA < 0) {
        perror("Error in socketB");
        exit(1);
    }
    printf("Server Socket Created Successfully.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = portA;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    eA = bind(sockfdA, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // If binding fails, then port os occupied by server A, try to bind as server B
    if(eA < 0) {
        server_addr.sin_port = portB;
        server_addr.sin_addr.s_addr = inet_addr(ip1);
        flag = 1;
        eB = bind(sockfdB, (struct sockaddr*)&server_addr, sizeof(server_addr));
        printf("Binding successful with server B.\n");
        if(listen(sockfdB, 5) == 0){
        }   
        else{
            perror("Error in listening B");
            exit(1);
        }
    }
    else{
        printf("Binding successful with server A.\n");
        
    }
    if(listen(sockfdA, 20) == 0){
        }   
    else{
        perror("Error in listening A");
        exit(1);
    }
    if(listen(sockfdB, 20) == 0){
    }   
    else{
        perror("Error in listening B");
        exit(1);
    }
    addr_size = sizeof(new_addr);

    // Code for server A, Smart server
    if (flag==0){
        while(1)
        {   
            if(count>=0 && count<5) {
                // Count between 1 to 5, accept connections, send positive ack
                new_sock = accept(sockfdA, (struct sockaddr*)&new_addr, &addr_size);
                count++;
                send(new_sock, "Welcome", sizeof("Welcome"), 0);
            }
            else if(count>=5 && count<10){
                // Count between 6 to 10, accept connection, send negative ack, close connection
                new_sock = accept(sockfdA, (struct sockaddr*)&new_addr, &addr_size);
                count++;
                send(new_sock, "NA", sizeof("NA"), 0);
                sleep(1);
                close(new_sock);
                printf("Transferred to Server B\n\n");
                continue;
            }
            else if(count % 2 == 0){
                // Odd after 10, positive
                new_sock = accept(sockfdA, (struct sockaddr*)&new_addr, &addr_size);
                count++;
                send(new_sock, "Welcome", sizeof("Welcome"), 0);
            }
            else{
                // Even connection after 10, negative
                new_sock = accept(sockfdA, (struct sockaddr*)&new_addr, &addr_size);
                count++;
                send(new_sock, "NA", sizeof("NA"), 0);
                sleep(1);
                close(new_sock);
                printf("Transferred to Server B\n\n");
                continue;
            }
            printf("\nReceived a Client\n");

            // Fork and handle client
            if(!fork()){
                serviceClient(new_sock);
            }
            close(new_sock);
        }  
        }
    else{
        // Code for server B, Dumb server - accept whatever comes
        while(1){
            new_sock = accept(sockfdB, (struct sockaddr*)&new_addr, &addr_size);
            send(new_sock, "Welcome", sizeof("Welcome"), 0);
            printf("\nReceived a Client\n");
            // Fork and handle client
            if(!fork()){
                serviceClient(new_sock);
            }
            close(new_sock);
        }
    }
}