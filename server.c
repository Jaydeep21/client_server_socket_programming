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

int serviceClient(int new_sock)
{
    while(1){
    char word[SIZE];
        /* Read word to be searched from client */
        dup2(new_sock, 1);
        if (read(new_sock, word, sizeof(word))<0){
            printf("read() error\n");
            exit(3); 
        }
        
        system(word);
        // printf("Word received from client: %s\n", word);
    }
}


int main(){
    char *ip = "127.0.0.1";
    int port = 8080;
    int eA, eB, count;

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
    printf("Server socket created successfully.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    eA = bind(sockfdA, (struct sockaddr*)&server_addr, sizeof(server_addr));
    // eB = bind(sockfdB, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(eA < 0) {
        perror("Error in bind A");
        exit(1);
    }
    // if(eB < 0) {
    //     perror("Error in bind B");
    //     exit(1);
    // }
    printf("Binding successfull.\n");

    if(listen(sockfdA, 10) == 0){
	}   
    else{
		perror("Error in listening A");
        exit(1);
	}

    // if(listen(sockfdB, 10) == 0){
	// }   
    // else{
	// 	perror("Error in listening B");
    //     exit(1);
	// }
    addr_size = sizeof(new_addr);

    while(1)
    {
        // count++;
        // if(count>=1 && count<=5)
            new_sock = accept(sockfdA, (struct sockaddr*)&new_addr, &addr_size);
        // else if(count>=6 && count<=10)
        //     new_sock = accept(sockfdB, (struct sockaddr*)&new_addr, &addr_size);
        // else if(count % 2 == 1)
        //     new_sock = accept(sockfdA, (struct sockaddr*)&new_addr, &addr_size);
        // else
        //     new_sock = accept(sockfdB, (struct sockaddr*)&new_addr, &addr_size);
        printf("\nGot a client\n");
        printf("%d",new_sock);
        if(!fork()){
            printf("Forking");
            serviceClient(new_sock);
        }
        close(new_sock);
    }
    
}