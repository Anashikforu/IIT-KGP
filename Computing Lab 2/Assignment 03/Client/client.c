
/*
Author ---  Md Ashik Khan
ID --- 21CS60A02
task ---    CL2 ASSIGNMENT 03
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<ctype.h>
#include <dirent.h>
#define MAX 1024
#define STR 256
#define PORT 8888

void upload(char filename[],int clientSocket);
void download(char filename[],int clientSocket);
void files(int clientSocket);
void users(int clientSocket);
void deleteIndex();
void insertIndex();
void readIndex();
void invite();

int main(){

	int clientSocket,fd;
	FILE *fp;
	char ch,c;
	struct sockaddr_in serverAddr;
    char msg[MAX],buffer[MAX];
    char str[MAX],str1[STR],str2[STR],str3[STR],str4[999];
	int n,len,count,j,size=0,b;
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Client Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	fd = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(fd< 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Connected to Server(type (/exit) to exit from server).\n");
    while(1){
		printf("Client: \t");
        bzero(str,sizeof(str));
	    n = 0; 
        while ((str[n++] = getchar()) != '\n') 
         ; 
        str[n-1]='\0';
        write(clientSocket,str,strlen(str));
        // len=strlen(str);
        // count=0;
        // for(j=0;j<len;j++)
        // {
        //     if(str[j]==' ')
        //         break;
        //     else
        //         str1[count++]=str[j];
        // }
        // str1[count]='\0';

        bzero(str1,sizeof(str1));  
        bzero(str2,sizeof(str2));
        sscanf(str,"%s %s",str1,str2);

        if(strcmp(str1,"/upload")==0)
        {
            upload(str2,clientSocket);
        }
        else if(strcmp(str1,"/download")==0)
        {
        	download(str2,clientSocket);
        }
        else if(strcmp(str,"/files")==0)
        {
            files(clientSocket);
        }
        else if(strcmp(str,"/users")==0)
        {
            users(clientSocket);
        }
        else if(strcmp(str1,"/delete")==0)
        {
        	bzero(buffer,sizeof(buffer));
        	send(clientSocket,buffer,sizeof(buffer),0);
        }
		else if(strcmp(str, "/exit") == 0){
			close(clientSocket);
			printf("[-]Disconnected from server.\n");
			exit(1);
		}
		
        bzero(msg,sizeof(msg));
		if(recv(clientSocket,msg, 1024, 0) < 0){
			printf("[-]Error in receiving data.\n");
		}else{
			printf("Server:  %s\n",msg);
			bzero(msg,sizeof(msg));
		}
	}
	return 0;
}

void upload(char filename[],int clientSocket){
    FILE *fp;
    int n,len,count,j,size=0,b;
    char buffer[MAX];
    b=0;
	fp=fopen(filename,"r");
	if(!fp)
	{
		printf("Error in opening.\n");
		exit(1);
	}
	while(fgets(buffer,sizeof(buffer),fp))
	{
		len=strlen(buffer);
		b=b+len;
	}
	send(clientSocket,&b,sizeof(b),0);
	rewind(fp); 
	
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

void download(char filename[],int clientSocket){
    FILE *fp;
    int n,len,count,j,size=0,b;
    char buffer[MAX];
    b=0;
    fp=fopen(filename,"w");
    if(!fp)
    {
    	printf("Error in the file.\n");
    	exit(1);
    }
    recv(clientSocket,&b,sizeof(b),0);
    bzero(buffer,sizeof(buffer));
    while(b>0)
    {
        bzero(buffer,sizeof(buffer));
        recv(clientSocket,buffer, MAX, 0);
        len=strlen(buffer);
        b=b-len;
        fprintf(fp,"%s",buffer);
    }
    send(clientSocket,&b,sizeof(b),0);
    fclose(fp);
}

/*
/files: View all ﬁles that have been uploaded to the server, along with all
details (owners, collaborators, permissions), and the no. of lines in the ﬁle.
*/
void files(int clientSocket){
    char buffer[MAX];
    int clientsd,b,addrlen;
    struct sockaddr_in address;

    recv(clientSocket,&b,sizeof(b),0);
    printf("Stored Files :  %d \n",b);

    bzero(buffer,sizeof(buffer));
    for (int i = 0; i < b; i++) 
    {
        recv(clientSocket,buffer, MAX, 0);
        printf("\t%d.%s\n" , i+1,buffer);
        bzero(buffer,sizeof(buffer));
    }
    send(clientSocket,&b,sizeof(b),0);
    bzero(buffer,sizeof(buffer));
}

/*
/users: View all active clients
*/
void users(int clientSocket){
    char buffer[MAX];
    int clientsd,b,addrlen;
    struct sockaddr_in address;

    recv(clientSocket,&b,sizeof(b),0);
    printf("Server:  Active Users: %d \n",b);

    bzero(buffer,sizeof(buffer));
    for (int i = 0; i < b; i++) 
    {
        recv(clientSocket,buffer, MAX, 0);
        printf("\t%d.%s\n" , i+1,buffer);
        bzero(buffer,sizeof(buffer));
    }
    send(clientSocket,&b,sizeof(b),0);
    bzero(buffer,sizeof(buffer));
}