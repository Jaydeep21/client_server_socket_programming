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

int serviceClient(int new_sock){ //this function handles all clients
    char opt[SIZE]; //to store output generated
    int saved_stdout = dup(1); // this will be used to quit the process and close server
    dup2(new_sock, 0); //set input stream to new socket descriptor from STDIN
    dup2(new_sock, 1); //set output stream to new socket descriptor from STDOUT
    dup2(new_sock, 2); //set error stream to new socket descriptor from STDERR
    char word[SIZE]; //to store input
    while(1){ //while client wants to execute command
        bzero(word, SIZE); //clearing input after every iteration
        bzero(opt, SIZE); //clearing output after every iteration
        if (read(0, word, sizeof(word))<0){ //reading message from default input(which is now socket ip)
            printf("read() error\n"); 
            exit(3); 
        }
        if (strncmp(word, "quit", 4) == 0) //if passed string is quit close the socket and kill child process
        {
            dup2(saved_stdout, 1);
            printf("Client Disconnected\n\n");
            close(new_sock); 
            exit(0);
        }
        printf("\n");
        system(word); //execute the command request from client
    }
}

int main(){
    char *ip = "192.168.2.124"; //setting ip address for serverA
    char *ip1 = "192.168.2.171"; //setting ip address for serverB
    int portA = htons(8080); //setting port for serverA
    int portB = htons(8081); //setting port for serverB
    int eA, eB, count = 0; //initializing eA and eB to store binding output & count for load balancing
    int flag = 0; // flag to select which server client should connect to 
    int sockfdA, sockfdB, new_sock; //socket for server A and B along with file descriptor of connected socket
    struct sockaddr_in server_addr, new_addr; //connection struct 
    socklen_t addr_size; //address length

    sockfdA = socket(AF_INET, SOCK_STREAM, 0); //socket for serverA
    sockfdB = socket(AF_INET, SOCK_STREAM, 0); //socket for serverB
    if(sockfdA < 0) { //error in socket A initialization
        perror("Error in socketA");
        exit(1);
    }
    if(sockfdA < 0) { //error in socket B initialization
        perror("Error in socketB");
        exit(1);
    }
    printf("Server Socket Created Successfully.\n");
    //setting connection params
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = portA;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    //binding serverA
    eA = bind(sockfdA, (struct sockaddr*)&server_addr, sizeof(server_addr));

    //if binding serverA fails connect to server B
    if(eA < 0) {
        //change ip and port as per serverB
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
    //setting up socketA listening capacity
    if(listen(sockfdA, 200) == 0){
        }   
    else{
        perror("Error in listening A");
        exit(1);
    }
    //setting up socketB listening capacity
    if(listen(sockfdB, 200) == 0){
    }   
    else{
        perror("Error in listening B");
        exit(1);
    }
    addr_size = sizeof(new_addr);

    if (flag==0){ //flag 0 indicates always connect to serverA and 1 indicates always connect to serverB 
        while(1) //always keep looking for clients
        {   
            new_sock = accept(sockfdA, (struct sockaddr*)&new_addr, &addr_size); //accpet the connection
            if(count>=0 && count<5) { //if current count is between 0 to 4 connect to server A
                count++; //to count number of clients connected
                send(new_sock, "Welcome", sizeof("Welcome"), 0); //send welcome message(positive acknowledgement)
            }
            else if(count>=5 && count<10){//if current count is between 5 to 9 connect to server A
                count++; //to count number of clients connected
                send(new_sock, "NA", sizeof("NA"), 0); //send NA negative acknowledgement
                sleep(1); //sleep for a second
                close(new_sock); //close the connection
                printf("Transferred to Server B\n\n");
                continue; //keep looking for new clients
            }
            else if(count % 2 == 0){//even number of client connection after 10(eg 10,12,14, ...)
                count++; //to count number of clients connected
                send(new_sock, "Welcome", sizeof("Welcome"), 0); //send welcome message(positive acknowledgement)
            }
            else{ //odd number of client connection after 10 (eg 11,13,15, ...)
                count++; //to count number of clients connected
                send(new_sock, "NA", sizeof("NA"), 0); //send NA negative acknowledgement
                sleep(1); //sleep for a second
                close(new_sock);  //close the connection
                printf("Transferred to Server B\n\n");
                continue; //keep looking for new clients
            }
            printf("\nReceived a Client\n");
            if(!fork()){ //let child process handle the client and parent goes back to look for new client
                serviceClient(new_sock);
            }
            close(new_sock); //close the socket in parent process
        }  
        }
    else{
        while(1){ //serverB is running and looking for client
            new_sock = accept(sockfdB, (struct sockaddr*)&new_addr, &addr_size); //accept any connection bcoz server A has evaluated the request
            send(new_sock, "Welcome", sizeof("Welcome"), 0); //send welcome message(positive acknowledgement)
            printf("\nReceived a Client\n");
            if(!fork()){ //let child process handle the client and parent goes back to look for new client
                serviceClient(new_sock);
            }
            close(new_sock); //close the socket in parent process
        }
    }
}