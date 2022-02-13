/*
Author ---  Md Ashik Khan
ID --- 21CS60A02
task ---    CL -2 ASSIGNMENT
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<ctype.h>
#include <regex.h>
#define MAX 1024
#define PORT 8080


/*
this function calculate how much char the file have and send this to clinet.
then clinet prepare for receiving the data. 
then send every char to client. 
*/
void uploadFIle(char filename[],int clientSocket){
			int len,b=0;
			FILE *fp;
			char buffer[MAX];
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
this function receive how much char the file have and prepare for receiving the data. 
then open the file and store the datas.
*/
void downloadFIle(char filename[],int clientSocket){
				FILE *fp;
				int len,b=0;
				char buffer[MAX];
				fp=fopen(filename,"w");
				if(!fp)
				{
					printf("Error in the file.\n");
					exit(1);
				}
				recv(clientSocket,&b,sizeof(b),0) ;
				bzero(buffer,sizeof(buffer));
				while(b>0)
				{
					bzero(buffer,sizeof(buffer));
					recv(clientSocket,buffer,MAX,0);
					len=strlen(buffer);
					b=b-len;
					fprintf(fp,"%s",buffer);
				}
				// printf("Server:  %s sorted successfully\n",filename);
				send(clientSocket,&b,sizeof(b),0);
				bzero(buffer,sizeof(buffer));
				fclose(fp);
}

int main(){

	int clientSocket,fd ;
	char ch,c;
	struct sockaddr_in serverAddr;
	char msg[MAX],str1[MAX],str2[MAX],str3[MAX],str4[MAX],str5[MAX],buffer[MAX],str[MAX];
	int n,len,count,count1,j,size=0,b,str_count,s;

	//validation 
    regex_t fileregex;
    int comp = regcomp(&fileregex, "^\\w+.(txt)$", REG_EXTENDED | REG_NOSUB) ;

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
	// printf("[+]Connected to Server(type (/exit) to exit from server).\n");
    bzero(msg,sizeof(msg));
    if(recv(clientSocket,msg, MAX, 0) < 0){
    	printf("[-]Error in receiving data.\n");
    }else{
        if(strcmp(msg,"limit_exceed") == 0){
            printf("Client limit exceed! Wait for a while.\n");
            close(clientSocket);
            exit(1);
        }else{
            printf("%s\n",msg);
        }
    	bzero(msg,sizeof(msg));
    }
    

    while(1){
		printf("Client: \t");
	    n = 0; 
        while ((str[n++] = getchar()) != '\n') 
         ; 
        str[n-1]='\0';
        write(clientSocket,str,strlen(str));
        str_count = sscanf(str,"%s %s %s %s %s",str1,str2,str3,str4,str5);
        
        if(strcmp(str1,"/sort")==0 && !regexec(&fileregex, str2, 0 , 0 , 0) && (strcmp(str3,"date") == 0 || strcmp(str3,"item_name") == 0 || strcmp(str3,"price") == 0 ) && str_count == 3)
        {
			uploadFIle(str2,clientSocket);
        	
        }
        else if(strcmp(str1,"/merge")==0 && !regexec(&fileregex, str2, 0 , 0 , 0) && !regexec(&fileregex, str3, 0 , 0 , 0) && !regexec(&fileregex, str4, 0 , 0 , 0) && (strcmp(str5,"date") == 0 || strcmp(str5,"item_name") == 0 || strcmp(str5,"price") == 0 ))
    	{
			uploadFIle(str2,clientSocket);
			uploadFIle(str3,clientSocket);
	    }
	    else if(strcmp(str1,"/similarity")==0 && !regexec(&fileregex, str2, 0 , 0 , 0) && !regexec(&fileregex, str3, 0 , 0 , 0) && str_count == 3)
	    {
			uploadFIle(str2,clientSocket);
			uploadFIle(str3,clientSocket);
	    }
		else if(strcmp(str, "/exit") == 0){
			close(clientSocket);
			printf("[-]Disconnected from server.\n");
			exit(1);
		}
		
        bzero(msg,sizeof(msg));
		if(recv(clientSocket,msg, MAX, 0) < 0){
			printf("[-]Error in receiving data.\n");
		}else{
			if(strncmp(msg,"download",8) == 0){
				sscanf(msg,"download %s\n",str2);
				downloadFIle(str2,clientSocket);
				bzero(msg,sizeof(msg));
				recv(clientSocket,msg, MAX, 0);
				printf("Server:  %s\n",msg);
			}
			else if(strncmp(msg,"similar",6) == 0){
				sscanf(msg,"similar %d\n",&s);
				printf("Server:  Similarity \n");
				for(int i=0; i<s; i++)
				{	
					bzero(msg,sizeof(msg));
					recv(clientSocket,msg,MAX,0);
					printf("%s\n",msg);
				}
				bzero(msg,sizeof(msg));
				recv(clientSocket,msg, MAX, 0);
				printf(" %s\n",msg);
			}else{
				printf("Server:  %s\n",msg);
			}
		}
		bzero(msg,sizeof(msg));
	}
}
	