
/*
Author ---  Md Ashik Khan
ID --- 21CS60A02
task ---    CL2 ASSIGNMENT 03
*/

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
#define UNIQUEID 10000
#define MAX 1000
#define STR 256
#define PORT 8888
#define CLIENT 5


/*
the server generates a unique 5-digit ID
w.r.t. client socket id, and stores the client details in a client records ﬁle/data
structure.
*/
struct clientRecord {
  int unique_id;
  int socket_id;
  int status;
} ;

/*
The client successfully uploading a ﬁle is called the owner of the given ﬁle. The
server should maintain a track of all ﬁles, the no. of lines in the ﬁle and owners in a permission records
ﬁle/data structure .
*/
struct permissionRecord {
  int owner;
  int lines;
  char file_name[STR];
} ;

struct permissionRecord fileRecord[MAX];
int stored_file = 0;

void upload(char filename[],int clientSocket,int owner);
void download(char filename[],int clientSocket);
void files(int clientSocket);
void updateFileRecord(char filename[],int clientSocket);
void users(struct clientRecord client_details[],int connected_client,int clientSocket);
void deleteIndex();
void insertIndex();
void readIndex();
void invite();
int uniqueIdGenerator(int unique_number ,struct clientRecord client_details[]);

int main(int argc , char *argv[])
{
    FILE *fp;
    int opt=1,n,i,j,count=0,len,b;
    int sock_fd , addrlen ,newsock_fd,connected_client = 0, activity , valread ,clientsd,owner;
    struct clientRecord client_details[CLIENT];
    int max_sd,size,del;
    char msg[MAX],buffer[MAX];
    char str[MAX],str1[STR],str2[STR],str3[STR],str4[999];
    struct sockaddr_in address;
    struct dirent *de;
    fd_set readfds;

    for (i = 0; i < CLIENT; i++) 
    {
        client_details[i].unique_id = 0;
        client_details[i].socket_id = 0;
        client_details[i].status = 0;
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
            clientsd = client_details[i].socket_id;
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
                if( client_details[i].socket_id == 0 )
                {
                    client_details[i].unique_id = uniqueIdGenerator( UNIQUEID ,client_details);;
                    client_details[i].socket_id =newsock_fd;
                    client_details[i].status = 1;
                    printf("Adding to list of sockets as %d\n" , i);
                    connected_client++;
                    break;
                }
            }
        }

        for (i = 0; i < CLIENT; i++) 
        {
            clientsd = client_details[i].socket_id;
            owner   = client_details[i].unique_id;

            if (FD_ISSET( clientsd , &readfds)) 
            {

                bzero(str,sizeof(str));
                if ((valread = read( clientsd ,str, 1024)) == 0)
                {
                    getpeername(clientsd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                    close(clientsd);
                    client_details[i].unique_id = 0;
                    client_details[i].socket_id = 0;
                    client_details[i].status = 0;

                    connected_client--;
                    break;
                }
                
                printf("\n");

                // n=strlen(str);
                // count=0;
                // for(j=0;j<n;j++)
                // {
                //     if(str[j]==' ')
                //         break;
                //     else
                //         str1[count++]=str[j];
                // }
                // str1[count]='\0';
                
                bzero(str1,sizeof(str1));  
                bzero(str2,sizeof(str2));  
                bzero(str3,sizeof(str3));  
                bzero(str4,sizeof(str4));  
                sscanf(str,"%s %s %s %s",str1,str2,str3,str4);

                printf("%s command received from client.\n",str);
                if(strcmp(str1,"/upload")==0)
                {
                    upload(str2,clientsd,owner);
                    printf("File %s stored Successfully.\n",str2);
                    strcpy(msg,"Successfully uploaded the file.\n");
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
                    printf("Client disconnected.\n");
                    bzero(msg,sizeof(msg));
                    client_details[i].unique_id = 0;
                    client_details[i].socket_id = 0;
                    client_details[i].status = 0;
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

void upload(char filename[],int clientSocket,int owner){
        FILE *fp;
        int n=0,len,count,count1,j,size=0,b;
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
            memset(buffer, 0x00, MAX); // clean buffer
            recv(clientSocket,buffer,sizeof(buffer),0);
            len=strlen(buffer);
            b=b-len;
            fprintf(fp,"%s",buffer);
        }

        send(clientSocket,&b,sizeof(b),0);
        bzero(buffer,sizeof(buffer));

        fclose(fp);

        updateFileRecord(filename,owner);
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
            exit(1);
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

/*
/files: View all ﬁles that have been uploaded to the server, along with all
details (owners, collaborators, permissions), and the no. of lines in the ﬁle.
*/
void files(int clientSocket){
    char buffer[MAX];
    int count=0,clientsd,addrlen,b;
    b = stored_file;
    struct sockaddr_in address;
    addrlen = sizeof(address);
    send(clientSocket,&b,sizeof(b),0);

    for (int i = 0; i < b; i++) 
    {
            sprintf(buffer,"File %s,owner %d ,line %d \n",fileRecord[i].file_name,fileRecord[i].owner,fileRecord[i].lines);
            send(clientSocket,buffer,strlen(buffer),0);
            bzero(buffer,sizeof(buffer));
    }
    recv(clientSocket,&b,sizeof(b),0);
    bzero(buffer,sizeof(buffer));
}

/*
The client successfully uploading a ﬁle is called the owner of the given ﬁle. The
server should maintain a track of all ﬁles and owners in a permission records
ﬁle/data structure .
*/
void updateFileRecord(char filename[],int owner){
    FILE *fp;
    int n=0,len,count,count1,j,size=0,b;
    char buffer[MAX];
    b=0;
    fp=fopen(filename,"r");

    if(!fp)
    {
        printf("Error in the file.\n");
        exit(1);
    }

    while(!feof(fp))
    {
        memset(buffer, 0x00, MAX); // clean buffer
        fscanf(fp, "%[^\n]\n",buffer); // read file *prefer using fscanf
        n++;
    }

    strcpy(fileRecord[stored_file].file_name,filename);
    fileRecord[stored_file].owner = owner;
    fileRecord[stored_file].lines = n;
    stored_file++;

    fclose(fp);
}
/*
/users: View all active clients
*/
void users(struct clientRecord client_details[],int connected_client,int clientSocket){
    char buffer[MAX];
    int count=0,b=connected_client,clientsd,addrlen;
    struct sockaddr_in address;
    addrlen = sizeof(address);
    send(clientSocket,&b,sizeof(b),0);

    for (int i = 0; i < CLIENT; i++) 
    {
        if(client_details[i].status !=0){   
            clientsd = client_details[i].socket_id;
            getpeername(clientsd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
            sprintf(buffer,"Client %d, ip %s , port %d, status Active \n",client_details[i].unique_id , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
            send(clientSocket,buffer,strlen(buffer),0);
            bzero(buffer,sizeof(buffer));
        }
    }
    recv(clientSocket,&b,sizeof(b),0);
    bzero(buffer,sizeof(buffer));
}

/*
the server generates a unique 5-digit ID
*/
int uniqueIdGenerator(int unique_number ,struct clientRecord client_details[]){

    for (int i = 0; i < CLIENT; i++) 
    {
        if(unique_number == client_details[i].unique_id){
            if(unique_number < 99999){
                unique_number++;
            }else{
                unique_number = 10000;
            }
            uniqueIdGenerator(unique_number ,client_details);
        }
    }

    return unique_number;
}