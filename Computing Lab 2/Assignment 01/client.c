/*	Assignment no.1
    CLIENT
	Author : Md. Ashik Khan
	Roll-no :21CS60A02

    To compile client program gcc client.c -o client
    To run client program ./client
*/
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include<stdlib.h>
#define MAX 1000 
#define PORT 8090 
void func(int sock_fd) 
{ 
    char msg[MAX]; 
    int n; 
    for (;;) { 
        bzero(msg, sizeof(msg)); 
        printf("\nEnter the command : "); 
        n = 0; 
        while ((msg[n++] = getchar()) != '\n') 
            ; 
        write(sock_fd, msg, sizeof(msg)); 
        bzero(msg, sizeof(msg));                   
        read(sock_fd, msg, sizeof(msg)); 
        printf("From Server : %s", msg); 
        if ((strncmp(msg, "exit", 4)) == 0) { 
            printf("Client Exit...\n"); 
            break; 
        } 
    } 
} 
  
int main() 
{ 
    int sock_fd, connfd; 
    struct sockaddr_in servaddr, cli; 
    sock_fd = socket(AF_INET, SOCK_STREAM, 0); //create a socket for the TCP client
    if (sock_fd == -1) { 
        printf("creation of socket is failed....\n");   //gives error 
        exit(EXIT_FAILURE); 
    } 
    else
        printf("socket created successfully..\n"); 
    bzero(&servaddr, sizeof(servaddr));  
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //localhost
    servaddr.sin_port = htons(PORT); 
    if (connect(sock_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {  //connection established
        printf("server connection failed...\n"); 
        exit(EXIT_FAILURE); 
    } 
    else
        printf("connected to the server..\n"); 
    func(sock_fd);    //call a function to send message to server
    close(sock_fd); 
}
