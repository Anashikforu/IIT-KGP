
/*
Author ---  Md Ashik Khan
ID --- 21CS60A02
task ---    CL2 ASSIGNMENT 03 Client Part
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#define MAX 1024
#define STR 256
#define PORT 8888

void upload(char filename[],int clientSocket);
void download(char filename[],int clientSocket);
void files(int clientSocket);
void users(int clientSocket);
void readIndex(int clientSocket);

int validateCommand(char command[]);
int fileExits(char filename[]);

int main(){

	int clientSocket,fd,invite_owner;
	FILE *fp;
	char ch,c;
	struct sockaddr_in serverAddr;
    char msg[MAX],buffer[MAX];
    char str[MAX],str1[STR],str2[STR],str3[STR],str4[STR];
	int n,len,count,j,size=0,b,validation,start_idx,end_idx;
    fd_set readfds;

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}

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

	        printf("[+]Client Socket is created.\n");
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
            bzero(buffer,sizeof(buffer));
            bzero(msg,sizeof(msg));
            bzero(str1,sizeof(str1));  
            bzero(str2,sizeof(str2));
            bzero(str3,sizeof(str3));
            bzero(str4,sizeof(str4));

            if(recv(clientSocket,msg, MAX, 0) < 0){
				printf("[-]Error in receiving data.\n");
			}
			
            int valid = sscanf(msg,"%s %s %d %s\n",str1,str2,&invite_owner,str4);

            if(strcmp(str4,"E") == 0){
                sprintf(str3,"EDITOR");
            }
            else if(strcmp(str4,"V") == 0){
                sprintf(str3,"VIEWER");
            }

			
			if (strcmp(str1, "/invite") == 0)
			{
                
                printf("Server:  CLIENT %d is inviting you as %s of %s\n", invite_owner,str3,str2);
				
				fprintf(stdout, "Valid Response: [Yes] [No] \n");
				bzero(str3, strlen(str3));
				bzero(msg, strlen(msg));
				n = 0; 
                while ((str3[n++] = getchar()) != '\n') 
                ; 
                str3[n-1]='\0';
                sprintf(msg,"/response %s %s %s",str2,str4,str3);
                send(clientSocket, msg, MAX, 0);
				
				printf("Server:  Response sent succesfully.\n");
			}
            else if(strlen(msg) == 0){
                close(clientSocket);
                printf("[-]Disconnected from server.\n");
                exit(1);
            }
            else{
                printf("Server:  %s\n", msg);
            }

			fflush(stdout);
			// printf("Client: \t");
        }

        if (FD_ISSET( 0 , &readfds)) 
        {
            // printf("Client: \t");
            bzero(str,sizeof(str));
            n = 0; 
            while ((str[n++] = getchar()) != '\n') 
            ; 
            str[n-1]='\0';
            write(clientSocket,str,strlen(str));

            bzero(str1,sizeof(str1));  
            bzero(str2,sizeof(str2));
            validation = sscanf(str,"%s %s %d %d",str1,str2,&start_idx,&end_idx);

            if(strcmp(str1,"/upload")==0 && validateCommand(str) == 1 && validation == 2)
            {
                if(fileExits(str2) == 1){
                    bzero(buffer,sizeof(buffer));
                    strcpy(buffer,"EXIST");
                    send(clientSocket,buffer,strlen(buffer),0);
                    

                    bzero(buffer,sizeof(buffer));
                    recv(clientSocket,buffer, MAX, 0);
                    if(strcmp(buffer,"Send")==0){
                        upload(str2,clientSocket);
                    }else if(strcmp(buffer,"Duplicate")==0){

                    }
                }else{
                    bzero(buffer,sizeof(buffer));
                    strcpy(buffer,"WRONG");
                    send(clientSocket,buffer,strlen(buffer),0);
                }
            }
            else if(strcmp(str1,"/download")==0 && validateCommand(str) == 1 && validation == 2)
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
            else if(strcmp(str1,"/read")==0 && validateCommand(str) == 1)
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
        usleep(9000);
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
    int clientsd,b,d,addrlen;
    struct sockaddr_in address;

    recv(clientSocket,&b,sizeof(b),0);
    printf("Server:  \n\tStored Files :  %d \n",b);

    bzero(buffer,sizeof(buffer));

    for (int i = 0; i < b; i++) 
    {
        recv(clientSocket,buffer, MAX, 0);
        printf("\t%d.%s\n" , i+1,buffer);
        bzero(buffer,sizeof(buffer));

        recv(clientSocket,&d,sizeof(d),0);
        printf("\t\tCollaborators: %d \n",d);

        for (int j = 0; j < d; j++) 
        {
            recv(clientSocket,buffer, MAX, 0);
            printf("\t\t%d.%s\n" , j+1,buffer);
            bzero(buffer,sizeof(buffer));
        }
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

/*
Validate the client inputed command
*/
int validateCommand(char command[]){

    char str1[STR],str2[STR],str3[STR],str4[STR];
    int start_idx,end_idx,valid,readVal,upVAl;

    valid = sscanf(command,"%s %s %s %[^\n]",str1,str2,str3,str4);

    if((strcmp(str1,"/upload") || strcmp(str1,"/download")) && valid == 2){
        return 1;
    }
    else if((strcmp(str1,"/read") || strcmp(str1,"/delete")) && valid >= 2){
        if(valid == 2){
            return 1;
        }else if(valid == 3){
            readVal = sscanf(command,"%s %s %d %[^\n]",str1,str2,&start_idx,str3);
            if(readVal == 3){
                return 1;
            }else{
                return 0;
            }
        }else if(valid == 4){
            readVal = sscanf(command,"%s %s %d %d %[^\n]",str1,str2,&start_idx,&end_idx,str3);
            if(readVal == 4){
                return 1;
            }else{
                return 0;
            }
        }
    }
    else{
        return 0;
    }

}

/*
Check to be uploaded file exits or not . 
*/
int fileExits(char filename[]){
    FILE *fp;
	fp=fopen(filename,"r");
	if(!fp)
	{
		return 0;
	}
    return 1;
}