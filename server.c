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

int serviceClient(int new_sock){
    // FILE *fp;
    char opt[SIZE];
    int saved_stdout = dup(1);
    dup2(new_sock, 0);
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
        if (strncmp(word, "quit", 4) == 0)
        {
            dup2(saved_stdout, 1);
            printf("Client Disconnected\n\n");
            close(new_sock); 
            // kill(getpid(), SIGKILL);
            exit(0);
        }
        // fp = popen(word, "r");
        // while ( fgets( opt, SIZE, fp ) != NULL )/* read from command */
        //     printf("%s", opt); /* print data */
        // pclose( fp );
        system(word);
    }
}

int main(){
    // char *ip = "127.0.0.1";
    int portA = htons(8080);
    int portB = htons(8081);
    int eA, eB, count = 0;
    int flag = 0;
    int sockfdA, sockfdB, new_sock;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    char buffer[SIZE];

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
    server_addr.sin_addr.s_addr = INADDR_ANY;

    eA = bind(sockfdA, (struct sockaddr*)&server_addr, sizeof(server_addr));

    if(eA < 0) {
        server_addr.sin_port = portB;
        // int optval = 1;
        // setsockopt(sockfdB, SOL_SOCKET, SO_REUSEPORT, (char *)&optval, sizeof(optval));
        // setsockopt(sockfdB, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
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

    if (flag==0){
        while(1)
        {   
            count++;
            if(count>=1 && count<=5) {
                // printf("Server A 1 %d\n", count);
                new_sock = accept(sockfdA, (struct sockaddr*)&new_addr, &addr_size);
                send(new_sock, "Welcome", sizeof("Welcome"), 0);
            }
            else if(count>=6 && count<=10){
                // printf("Server B 1 %d\n", count);
                new_sock = accept(sockfdA, (struct sockaddr*)&new_addr, &addr_size);
                send(new_sock, "No", sizeof("No"), 0);
                sleep(1);
                close(new_sock);
                printf("Transferred to Server B\n\n");
                continue;
            }
            else if(count % 2 == 1){
                // printf("Server A 2 %d\n", count);
                new_sock = accept(sockfdA, (struct sockaddr*)&new_addr, &addr_size);
                send(new_sock, "Welcome", sizeof("Welcome"), 0);
            }
            else{
                // printf("Server B 2 %d\n", count);
                new_sock = accept(sockfdA, (struct sockaddr*)&new_addr, &addr_size);
                send(new_sock, "No", sizeof("No"), 0);
                sleep(1);
                close(new_sock);
                printf("Transferred to Server B\n\n");
                continue;
            }
            printf("\nReceived a Client\n");
            // printf("%d",new_sock);
            if(!fork()){
                serviceClient(new_sock);
            }
            close(new_sock);
        }  
        }
    else{
        while(1){
            new_sock = accept(sockfdB, (struct sockaddr*)&new_addr, &addr_size);
            send(new_sock, "Welcome", sizeof("Welcome"), 0);
            printf("\nReceived a Client\n");
            // printf("%d",new_sock);
            if(!fork()){
                serviceClient(new_sock);
            }
            close(new_sock);
        }
    }
}