
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
void readIndex(int clientSocket);
void invite();

void print_error(char *msg)
{
	perror(msg);
	exit(0);
}
void print_msg(char *msg)
{
	puts(msg);
}

int main(){

	int clientSocket,fd,invite_owner;
	FILE *fp;
	char ch,c;
	struct sockaddr_in serverAddr;
    char msg[MAX],buffer[MAX];
    char str[MAX],str1[STR],str2[STR],str3[STR],str4[999];
	int n,len,count,j,size=0,b,validation;
    fd_set readfds;

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

    if(recv(clientSocket,msg, MAX, 0) < 0){
    	printf("[-]Error in receiving data.\n");
    }
    else{
        if(strcmp(msg,"limit_exceed") == 0){
            printf("Client limit exceed! Wait for a while.\n");
            close(clientSocket);
            exit(1);
        }
        else{
            printf("%s\n",msg);
        }
    	bzero(msg,sizeof(msg));
    }

	// printf("[+]Connected to Server(type (/exit) to exit from server).\n");

    while(1){

        int fdmax = clientSocket;
        FD_ZERO(&readfds);
		FD_SET(clientSocket, &readfds); /* add sockfd to connset */
		FD_SET(0, &readfds);			 /* add STDIN to connset */

		if (select(fdmax + 1, &readfds, NULL, NULL, NULL) < 0)
		{
			fprintf(stdout, "select() error\n");
			exit(0);
		}

        if (FD_ISSET( clientSocket , &readfds)) 
        {
            bzero(msg,sizeof(msg));
            bzero(str1,sizeof(str1));  
            bzero(str2,sizeof(str2));
            bzero(str2,sizeof(str2));
            bzero(str2,sizeof(str4));

            if(recv(clientSocket,msg, MAX, 0) < 0){
				printf("[-]Error in receiving data.\n");
			}
			
            int valid = sscanf(msg,"%s %s %d %s\n",str1,str2,invite_owner,str4);

            if(strcmp(str4,"E") == 1){
                strcpy(str4,"EDITOR");
            }
            else if(strcmp(str4,"V") == 1){
                strcpy(str4,"VIEWER");
            }
            printf("CLIENT %d is inviting you as %s of %s\n", invite_owner,str4,str2);
			
			if (strcmp(str1, "/invite") == 0)
			{
				//printf("%s\n", msger);
				fprintf(stdout, "Reply in Yes and No only\n");
				bzero(msg, strlen(msg));
				fgets(msg, 1024, stdin);
				if (strncmp(msg, "Yes", 3) == 0 || strncmp(msg, "No", 2) == 0)
				{
					if (send(clientSocket, msg, MAX, 0) < 0)
					{
                        perror("[-]Error in sending data.\n");
	                    exit(0);
					}
				}
				else
				{
					if (send(clientSocket, msg, MAX, 0) < 0)
					{
						perror("[-]Error in sending data.\n");
	                    exit(0);
					}
				}
				printf("feedback succesfully sended \n");
			}

			fflush(stdout);
			// printf("Client2: \t");
        }

        if (FD_ISSET( 0 , &readfds)) 
        {
            // printf("Client1: \t");
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
            validation = sscanf(str,"%s %s",str1,str2);

            if(strcmp(str1,"/upload")==0 && validation == 2)
            {
                bzero(buffer,sizeof(buffer));
                recv(clientSocket,buffer, MAX, 0);
                if(strcmp(buffer,"Send")==0){
                    upload(str2,clientSocket);
                }else if(strcmp(buffer,"Duplicate")==0){

                }
            }
            else if(strcmp(str1,"/download")==0 && validation == 2)
            {
                bzero(buffer,sizeof(buffer));
                recv(clientSocket,buffer, MAX, 0);
                if(strcmp(buffer,"permission_granted")==0){
                    download(str2,clientSocket);
                }else if(strcmp(buffer,"permission_denied")==0){

                }
            }
            else if(strcmp(str,"/files")==0)
            {
                files(clientSocket);
            }
            else if(strcmp(str,"/users")==0)
            {
                users(clientSocket);
            }
            else if(strcmp(str1,"/read")==0)
            {
                bzero(buffer,sizeof(buffer));
                recv(clientSocket,buffer, MAX, 0);
                if(strcmp(buffer,"permission_granted")==0){
                    readIndex(clientSocket);
                }else if(strcmp(buffer,"permission_denied")==0){

                }
            }
            else if(strcmp(str, "/exit") == 0){
                close(clientSocket);
                printf("[-]Disconnected from server.\n");
                exit(1);
            }

            bzero(buffer,sizeof(buffer));
            bzero(msg,sizeof(msg));

            if(recv(clientSocket,msg, MAX, 0) < 0){
                printf("[-]Error in receiving data.\n");
            }else{
                printf("Server:  %s\n",msg);
                bzero(msg,sizeof(msg));
            }
        }
    }
	return 0;
}

/*
/upload <filename>: Upload the local ﬁle ﬁlename to the server
*/
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

/*
/download <filename>: Download the server ﬁle ﬁlename to the client, if
given client has permission to access that ﬁle
*/
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
    printf("Server:  \n\tStored Files :  %d \n",b);

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
Read from ﬁle ﬁlename
starting from line index start_idx to end_idx . If only one index is speciﬁed, read
that line. If none are speciﬁed, read the entire ﬁle.
*/
void readIndex(int clientSocket){
    char buffer[MAX];
    int clientsd,b,addrlen;

    recv(clientSocket,&b,sizeof(b),0);
    printf("Server:  \n\t lines :  %d \n",b);

    bzero(buffer,sizeof(buffer));
    for (int i = 0; i < b; i++) 
    {
        recv(clientSocket,buffer, MAX, 0);
        printf("\t%d.\t%s\n" , i+1,buffer);
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
    printf("Server:  \n\tActive Users: %d \n",b);

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