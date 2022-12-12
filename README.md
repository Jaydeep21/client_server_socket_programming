# client_server_socket_programming

> Client server implementation written in C with load balancing.

# Problem Statement

COMP 8567<br>
Fall 2022<br>
Project Marks 100

Write two C programs a client and a server to implement a remote shell connection using sockets.
The server process/s and the client process will run on two different machines and the
communication between the two processes is achieved using Sockets.<br>

## Server:<br>
* Two copies of the server (serverA and serverB) must start running before any of the client/s
and wait for connections. <br>
  * serverA and serverB can run on two different terminals of the same machine or
on two different machines.<br>
    * Please Note: serverA and serverB are identical copies of the same program
running on different terminals/machines<br>
* When either the server receives a request from a client, it forks and lets the child process (of
the server) take care of the client request in a separate function called ServiceClient(), while the
parent process goes back to wait for the next client.<br>

  * A server can receive requests from multiple client programs (running on different systems/ different shell terminals of the same system). Each request must lead to a new child process (of the server) that is created to service the request.<br>

### The Server’s Child Process:
1. uses " dup2()" to read from the client socket instead of standard input
2. gets in an infinite loop:
   * reads a shell command from the client’s socket,
   * if the client sends "quit", then the server’s child, closes socket and quits.
   * otherwise, it executes the command (You must execute the command without using any
    of the exec series of system calls)
 3. The results of the command must be transferred to the client.

## Client:
The client process connects to the server and gets into an infinite loop
 1. reads a command from keyboard,
 2. writes the command to the server,
 3. if command is "quit", closes socket, and quits
 4. otherwise, reads command output from the socket (received from the server) and displays it on the screen
<br>

### Additional Rules for serverA and serverB (very important):<br>

* The first 5 client connections are to be handled by serverA<br>
* The next 5 client connections are to be handled by serverB<br>
* The remaining client connections are to be handled by serverA and serverB in an
alternating fashion. (ex: connection 11 is to be handled by server A, connection 12
by serverB, 13 by serverA and so on)<br>
  * This is mainly to simulate load balancing

***

## How to execute

Execute below command to compile server:
```
gcc -o server server.c
```
Execute below command to compile client:
```
gcc -o client client.c
```
Execute two instances of server using
```
./server
```
Execute multiple instances of client as per your need
```
./client
```
***
# Solution Approach

## Server

* Server initializes IP and port for server A and B.
* For the first instance server A is binded with the given port and IP. 
* For the second instance server A execution will give error as port 
* Server A acts as master server and B acts as dumb server.
* Server A accepts the client connection and checks count value 
  * If it is supposed to be passed to server B it sends negative ack
  * Else sends positive acknowledgement
* Server B always accepts the connection as server A has evaluated the request.
* As the server(any A or B) accepts the request a child process is created. 
* Child funtion manages execution of normal bash commands using the system function. 
* Child process also changes its input, output, and error stream to new socket descriptor.

## Client 

* Initialize with 2 IP and ports of server A and B. 
* Creates socket instance and connects to server A.
* ServerA responds with positive or negative acknowledgement. 
* In case of positive acknowledgement it runs normally else it connects to server B. 
* In case if quit is passed close socket instance and exit.
 

 ***
 ## Our Team

- [Jaydeep Dharamsey](https://linkedin.com/in/jaydeepdharamsey/)
- [Raj Manoj Dedhia](https://linkedin.com/in/rajdedhia10/)