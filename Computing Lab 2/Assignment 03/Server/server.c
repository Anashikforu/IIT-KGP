#include<stdio.h>
#include<string.h>   
#include<stdlib.h>
#include<errno.h>
#include<math.h>
#include<unistd.h>   
#include<arpa/inet.h>   
#include<sys/types.h>
#include<ctype.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<time.h>
#include<sys/time.h>
#include <dirent.h>
#define MAX 1000
#define STR 256
#define PORT 8888
#define CLIENT 5

void upload(char filename[],int clientSocket);
void download(char filename[],int clientSocket);
void files(int clientSocket);
void users(int client_details[],int connected_client,int clientSocket);
void deleteIndex();
void insertIndex();
void readIndex();
void invite();


int main(int argc , char *argv[])
{
    FILE *fp;
    int opt=1,n,i,j,count=0,len,b;
    int sock_fd , addrlen ,newsock_fd, client_details[CLIENT] ,connected_client = 0, activity , valread ,clientsd;
    int max_sd,size,del;
    char msg[MAX],buffer[MAX];
    char str[MAX],str1[STR],str2[STR],str3[STR],str4[999];
    struct sockaddr_in address;
    struct dirent *de;
    fd_set readfds;
    for (i = 0; i < CLIENT; i++) 
    {
        client_details[i] = 0;
    }
    if( (sock_fd = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
    {
        printf("socket creation failed.\n");
        exit(1);
    }
    if( setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        printf("setsockopt");
        exit(1);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    if (bind(sock_fd, (struct sockaddr *)&address, sizeof(address))<0) 
    {
        printf("binding failed.\n");
        exit(1);
    }
    printf("[+]connected------\n");
    printf("[+]Listener on port %d \n", PORT);
    if (listen(sock_fd, 3) < 0)
    {
        printf("listening failed.\n");
        exit(1);
    }
    addrlen = sizeof(address);
    printf("Waiting for connections ...");
    while(1) 
    {
        FD_ZERO(&readfds);
        FD_SET(sock_fd, &readfds);
        max_sd =sock_fd;
        bzero(msg,sizeof(msg));
        for ( i = 0 ; i < CLIENT ; i++) 
        {
            clientsd = client_details[i];
            if(clientsd > 0)
                FD_SET( clientsd , &readfds);
            if(clientsd > max_sd)
                max_sd = clientsd;
        }
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
   
        if ((activity < 0) && (errno!=EINTR)) 
        {
            printf("select error");
        }
        if (FD_ISSET(sock_fd, &readfds)) 
        {
            if ((newsock_fd = accept(sock_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" ,newsock_fd, inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
            for (i = 0; i < CLIENT; i++) 
            {
                if( client_details[i] == 0 )
                {
                    client_details[i] =newsock_fd;
                    printf("Adding to list of sockets as %d\n" , i);
                    connected_client++;
                    break;
                }
            }
        }
        for (i = 0; i < CLIENT; i++) 
        {

            clientsd = client_details[i];
             
            if (FD_ISSET( clientsd , &readfds)) 
            {
                bzero(str,sizeof(str));
                if ((valread = read( clientsd ,str, 1024)) == 0)
                {
                    getpeername(clientsd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                    close(clientsd);
                    client_details[i] = 0;
                    break;
                }
                n=strlen(str);
                count=0;
                for(j=0;j<n;j++)
                {
                    if(str[j]==' ')
                        break;
                    else
                        str1[count++]=str[j];
                }
                str1[count]='\0';
                
                printf("%s command received from client.\n",str);
                if(strcmp(str1,"/upload")==0)
                {
                    upload(str2,clientsd);
                    printf("File stored Successfully.\n");
                    strcpy(msg,"Successfully received the file.\n");
                }
                else if(strcmp(str1,"/download")==0)
                {
                    download(str2,clientsd);
                    strcpy(msg,"Sending file to client.\n");
                }
                else if(strcmp(str,"/files")==0)
                {
                    files(clientsd);
                    strcpy(msg,"File listing completed.\n");
                }
                else if(strcmp(str,"/users")==0)
                {
                    users(client_details,connected_client,clientsd);
                    strcpy(msg,"User listing completed.\n");
                }
                else if(strcmp(str1,"/delete")==0)
                {
                    bzero(buffer,sizeof(buffer));
                    recv(clientsd,buffer,sizeof(buffer),0);
                    bzero(msg,sizeof(msg));
                    del=remove(str2);
                    if(!del)
                    {
                        printf("The file is deleted Successfully.\n");
                        strcpy(msg,"The file is deleted Successfully.\n");
                    }
                    else{
                        printf("File is not deleted.\n");
                        strcpy(msg,"File is not deleted.\n");
                    }
                }
                else if(strcmp(str,"/exit")==0)
                {
                    printf("/exit command received from client.\n");
                    printf("Client disconnected.\n");
                    bzero(msg,sizeof(msg));
                    client_details[i] = 0;
                    connected_client--;
                    strcpy(msg,"Disconnected from server.\n");
                }
                else
                {
                    printf("Wrong command received.\n");
                    bzero(msg,sizeof(msg));
                    strcpy(msg,"Wrong command.\n");
                }
                send(clientsd ,msg,strlen(msg),0);
                bzero(msg,sizeof(msg));                     
            
            }
        }
    }
    
    return 0;
}

void upload(char filename[],int clientSocket){
        FILE *fp;
        int n,len,count,count1,j,size=0,b;
        char buffer[MAX];
        b=0;
        fp=fopen(filename,"w");
        if(!fp)
        {
            printf("Error in the file.\n");
            return -1;
        }
        recv(clientSocket,&b,sizeof(b),0);
        bzero(buffer,sizeof(buffer));
        while(b>0)
        {
            bzero(buffer,sizeof(buffer));
            recv(clientSocket,buffer,sizeof(buffer),0);
            len=strlen(buffer);
            b=b-len;
            fprintf(fp,"%s",buffer);
        }
        send(clientSocket,&b,sizeof(b),0);
        bzero(buffer,sizeof(buffer));
        fclose(fp);
}

void download(char filename[],int clientSocket){
        FILE *fp;
        int n,len,count,count1,j,size=0,b;
        char buffer[MAX];
        b=0;
        fp=fopen(filename,"r");
        if(!fp)
        {
            printf("Error in the file.\n");
            return -1;
        }
        while(fgets(buffer,sizeof(buffer),fp))
        {
            len=strlen(buffer);
            b=b+len;
        }
        send(clientSocket,&b,sizeof(b),0);
        rewind(fp); 
        printf("Sending file to client.\n");
        while(fgets(buffer,sizeof(buffer),fp))
        {
            send(clientSocket,buffer,strlen(buffer),0);
            bzero(buffer,sizeof(buffer));
        }
        recv(clientSocket,&b,sizeof(b),0);
        bzero(buffer,sizeof(buffer));
        fflush(stdin);
        fclose(fp);
}

void files(int clientSocket){
    struct dirent *de;
    DIR *dr=opendir(".");
    int n,len,count,count1,j,size=0,b;
        char buffer[MAX];
    b=0;
    if(dr==NULL)
    {
        printf("Could not open current directory.\n");
    }
    while((de=readdir(dr))!=NULL)
    {
        bzero(buffer,sizeof(buffer));
        strcpy(buffer,de->d_name);
        buffer[len]='\t';
        len=strlen(buffer);
        b=b+len;
    }
    printf("/files command received from client.\n");
    send(clientSocket,&b,sizeof(b),0);
    closedir(dr);
    dr=opendir(".");
    printf("Sending list of files to client.\n");
    while((de=readdir(dr))!=NULL)
    {
        bzero(buffer,sizeof(buffer));
        strcpy(buffer,de->d_name);
        len=strlen(buffer);
        buffer[len]='\t';
        send(clientSocket,buffer,strlen(buffer),0);
    }
    recv(clientSocket,&b,sizeof(b),0);
}

void users(int client_details[],int connected_client,int clientSocket){
    char buffer[MAX];
    int count=0,b=connected_client,clientsd,addrlen;
    struct sockaddr_in address;

    send(clientSocket,&b,sizeof(b),0);

    for (int i = 0; i < CLIENT; i++) 
    {
        if(client_details[i] !=0){   
            clientsd = client_details[i];
            getpeername(clientsd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
            sprintf(buffer,"Client %n , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
            send(clientSocket,buffer,strlen(buffer),0);
            bzero(buffer,sizeof(buffer));
        }
    }
    recv(clientSocket,&b,sizeof(b),0);
    bzero(buffer,sizeof(buffer));
}