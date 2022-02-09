
/*
Author ---  Md Ashik Khan
ID --- 21CS60A02
task ---    CL2 ASSIGNMENT 03 Server Part
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
#define MAX 1024
#define STR 256
#define PORT 8888
#define CLIENT 2

/*
ﬁles that have been uploaded to the server, along with all
details (owners, collaborators, permissions)
*/
struct collaborator {
  int collaborator_id;
  int permission;     // 2 = editor , 3 = viewer
} ;

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
  struct collaborator collaborators[MAX];
  int total_collaborators;
  char file_name[STR];
} ;

struct permissionRecord fileRecord[MAX];
int stored_file = 0;
int UNIQUEID = 10000;
int allCLients[MAX];
int allClientsNo = 0;

void upload(char filename[],int clientSocket,int owner);
void download(char filename[],int clientSocket);
void files(int clientSocket);
void updateFileRecord(char filename[],int clientSocket);
void users(struct clientRecord client_details[],int connected_client,int clientSocket);
void readIndex(char filename[],int clientSocket,int start_idx,int end_idx);
void insertIndex(char filename[],int clientSocket,int idx,char message[]);
void deleteIndex(char filename[],int clientSocket,int start_idx,int end_idx);
void restore(char sourceFile[],char targetFile[]);
void updateFileLines(char filename[]);
void updateFileCollaborator(char filename[],int collaborator_id,int permission);

int uniqueIdGenerator(int unique_number ,struct clientRecord client_details[]);
int checkFileName(char filename[]);
int getFilelines(char filename[]);
int getFileOwnerFd(struct clientRecord client_details[],char filename[]);
int checkClientStatus(struct clientRecord client_details[],int client_id);
int getClientSoket(struct clientRecord client_details[],int client_id);
int checkFilePermission(char filename[],int clientSocket,int owner,int editorPermission);
int validateCommand(char command[]);

char *retriveMessage(char message[]);

int main(int argc , char *argv[])
{
    FILE *fp;
    int opt=1,n,i,j,count=0,len,b;
    int sock_fd , addrlen ,newsock_fd,connected_client = 0, activity , valread ,clientsd,owner;
    struct clientRecord client_details[CLIENT];
    int max_sd,size,del;
    char msg[MAX],buffer[MAX];
    char str[MAX],str1[STR],str2[STR],str3[STR],str4[STR];
    int start_idx,end_idx;
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
            if(connected_client < CLIENT){

                bzero(msg,sizeof(msg)); 

                printf("New connection , socket fd is %d , ip is : %s , port : %d \n" ,newsock_fd, inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                for (i = 0; i < CLIENT; i++) 
                {
                    if( client_details[i].socket_id == 0 )
                    {
                        int new_id = uniqueIdGenerator( UNIQUEID ,client_details);
                        client_details[i].unique_id = new_id;
                        client_details[i].socket_id =newsock_fd;
                        client_details[i].status = 1;
                        printf("Adding to list of CLients as %d\n" , new_id);

                        sprintf(msg,"[+]Adding to list of CLients as %d.\n[+]Connected to Server(type (/exit) to exit from server).\n",new_id);
                        send(newsock_fd ,msg,MAX,0);
                        bzero(msg,sizeof(msg)); 
                        connected_client++;
                        allCLients[allClientsNo] = new_id;
                        allClientsNo++;
                        break;
                    }
                }
            }
            else{

                bzero(msg,sizeof(msg)); 
                strcpy(msg,"limit_exceed");
                send(newsock_fd ,msg,MAX,0);
                bzero(msg,sizeof(msg)); 
            }
        }

        for (i = 0; i < CLIENT; i++) 
        {
            clientsd = client_details[i].socket_id;
            owner   = client_details[i].unique_id;

            if (FD_ISSET( clientsd , &readfds)) 
            {

                bzero(str,sizeof(str));
                if ((valread = read( clientsd ,str, MAX)) == 0)
                {
                    getpeername(clientsd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    printf("Client %d disconnected , ip %s , port %d \n" ,owner, inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                    close(clientsd);
                    client_details[i].unique_id = 0;
                    client_details[i].socket_id = 0;
                    client_details[i].status = 0;

                    connected_client--;
                    break;
                }
                
                printf("\n");

                
                bzero(str1,sizeof(str1));  
                bzero(str2,sizeof(str2));  
                bzero(str3,sizeof(str3));  
                bzero(str4,sizeof(str4));  

                int valid = sscanf(str,"%s %s %d %d",str1,str2,&start_idx,&end_idx);
                int editor_permission = 0;

                // editor permission = 1 for Insert and Delete
                // editor permission = 0 for Read and Download

                printf("%s command received from client.\n",str);

                if(strcmp(str1,"/upload")==0 && validateCommand(str) == 1 && valid == 2)
                {
                    bzero(buffer,sizeof(buffer));
                    recv(clientsd,buffer, MAX, 0);
                    if(strcmp(buffer,"EXIST")==0){
                      
                        if(checkFileName(str2) == 1){
                            strcpy(msg,"Send");
                            send(clientsd ,msg,MAX,0);
                            bzero(msg,sizeof(msg));

                            upload(str2,clientsd,owner);

                            printf("File %s stored Successfully.\n",str2);
                            strcpy(msg,"Successfully uploaded the file.\n");
                        }else{
                            strcpy(msg,"Duplicate");
                            send(clientsd ,msg,MAX,0);
                            bzero(msg,sizeof(msg));

                            printf("Duplicate Filename %s .\n",str2);
                            strcpy(msg,"Duplicate Filename.Please upload file with a unique filename.\n");
                        }

                    }else if(strcmp(buffer,"WRONG")==0){

                        printf("Error in opening File %s.\n",str2);
                        strcpy(msg,"Error in opening File .\n");
                    } 
                }
                else if(strcmp(str1,"/download")==0 && validateCommand(str) == 1 && valid == 2)
                {
                    editor_permission = 0;

                    if(checkFilePermission(str2,clientsd,owner,editor_permission) == 1){
                        strcpy(msg,"permission_granted");
                        send(clientsd ,msg,MAX,0);
                        bzero(msg,sizeof(msg));

                        download(str2,clientsd);
                        strcpy(msg,"Successfully Sent file to client.\n");
                    }else{
                        strcpy(msg,"permission_denied");
                        send(clientsd ,msg,MAX,0);
                        bzero(msg,sizeof(msg));

                        if(checkFileName(str2)){
                            printf("File %s does not exist.\n",str2);
                            strcpy(msg,"File does not exist.\n");
                        }else{
                            printf("Permission Denied for File %s .\n",str2);
                            strcpy(msg,"Permission Denied for File.\n");
                        }
                    }
                   
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
                else if(strcmp(str1,"/read")==0 && validateCommand(str) == 1)
                {
                    int total_lines = getFilelines(str2);
                    editor_permission = 0;

                    //If only one index is speciﬁed, read that line.
                    if(valid == 3){
                        end_idx = start_idx;
                    }

                    //If none are speciﬁed, read the entire ﬁle.
                    if(valid == 2){
                        start_idx = 0;
                        end_idx = total_lines - 1;
                    }

                    if(start_idx < 0 ){
                        start_idx += total_lines;
                    }

                    if(end_idx < 0 ){
                        end_idx += total_lines;
                    }

                    if(checkFilePermission(str2,clientsd,owner,editor_permission) == 1 && start_idx <= end_idx  && total_lines > start_idx && total_lines > end_idx && start_idx >= 0  && end_idx >= 0 && total_lines > 0){
                        strcpy(msg,"permission_granted");
                        send(clientsd ,msg,MAX,0);
                        bzero(msg,sizeof(msg));

                        readIndex(str2,clientsd,start_idx,end_idx);
                        strcpy(msg,"File reading completed.\n");
                    }else{
                        strcpy(msg,"permission_denied");
                        send(clientsd ,msg,MAX,0);
                        bzero(msg,sizeof(msg));

                        /*
                        Appropriate error message to be returned to client
                        based on situation (ﬁle does not exist / client does not have access /
                        invalid line numbers)
                        */
                        if(checkFileName(str2)){
                            printf("File %s does not exist.\n",str2);
                            strcpy(msg,"File does not exist.\n");
                        }
                        else if(checkFilePermission(str2,clientsd,owner,editor_permission) == 0){
                            printf("Client does not have access for File %s .\n",str2);
                            strcpy(msg,"Client does not have access for File.\n");
                        }
                        else if( total_lines == 0){
                            printf("File %s is empty.\n",str2);
                            strcpy(msg,"File is empty.\n");
                        }
                        else if(start_idx > end_idx  || total_lines <= start_idx || total_lines <= end_idx || start_idx < 0  || end_idx < 0){
                            printf("invalid line numbers for File %s .\n",str2);
                            strcpy(msg,"invalid line numbers for File.\n");
                        }
                        else{
                            printf("Wrong Command.\n");
                            strcpy(msg,"Wrong Command.\n");
                        }
                    }
                }
                else if(strcmp(str1,"/insert")==0 && valid >= 2)
                {  
                    int last_insert = 0, msglen;
                    char conv_msg[STR];
                    editor_permission = 1;

                    valid = sscanf(str,"%s %s %d %[^\n]",str1,str2,&start_idx,str4);

                    if(valid == 2){
                        valid = sscanf(str,"%s %s %[^\n]",str1,str2,str4);
                        last_insert = 1;
                    }

                    msglen = strlen(str4);

                    if(msglen < 3){
                        valid--;
                    }
                    else if(str4[0] == 34 && str4[msglen-1] == 34){
                        strcpy(conv_msg,retriveMessage(str4));
                    }
                    else{
                        valid--;
                    }

                    int total_lines = getFilelines(str2);

                    if(last_insert == 1){
                        start_idx = total_lines;
                    }

                    if(start_idx < 0 ){
                        start_idx += total_lines;
                    }

                    if((valid == 4 || (valid == 3 && last_insert == 1)) && total_lines >= start_idx && checkFilePermission(str2,clientsd,owner,editor_permission) == 1 ){
    
                            insertIndex(str2,clientsd,start_idx,conv_msg);
                            strcpy(msg,"File inserting completed.\n");

                    }else{
                        
                        /*
                        Appropriate error message to be returned to client
                        based on situation (ﬁle does not exist / client does not have access /
                        invalid line numbers)
                        */
                        if(checkFileName(str2)){
                            printf("File %s does not exist.\n",str2);
                            strcpy(msg,"File does not exist.\n");
                        }
                        else if(checkFilePermission(str2,clientsd,owner,editor_permission) == 0){
                            printf("Client does not have access for File %s .\n",str2);
                            bzero(msg,sizeof(msg));
                            strcpy(msg,"Client does not have access for File.\n");
                        }
                        else if(total_lines < start_idx){
                            printf("invalid line numbers for File %s .\n",str2);
                            bzero(msg,sizeof(msg));
                            strcpy(msg,"invalid line numbers for File.\n");
                        }
                        else if(strlen(str4) == 0){
                            printf("Empty message.\n");
                            strcpy(msg,"Empty message.\n");
                        }
                        else{
                            printf("Wrong Command\n");
                            bzero(msg,sizeof(msg));
                            strcpy(msg,"Wrong Command.Valid Command: /insert <filename> <idx> “<message>”\n");
                        }
                    }
                }
                else if(strcmp(str1,"/delete")==0 && validateCommand(str) == 1)
                {
                    int total_lines = getFilelines(str2);
                    editor_permission = 1;

                    //If only one index is speciﬁed, read that line.
                    if(valid == 3){
                        end_idx = start_idx;
                    }

                    //If none are speciﬁed, read the entire ﬁle.
                    if(valid == 2){
                        start_idx = 0;
                        end_idx = total_lines - 1;
                    }

                    if(start_idx < 0 ){
                        start_idx += total_lines;
                    }

                    if(end_idx < 0 ){
                        end_idx += total_lines;
                    }

                    if(checkFilePermission(str2,clientsd,owner,editor_permission) == 1 && start_idx <= end_idx  && total_lines > start_idx && total_lines > end_idx && start_idx >= 0  && end_idx >= 0 && total_lines > 0){
                        deleteIndex(str2,clientsd,start_idx,end_idx);
                        printf("Deletion completed for File %s .\n",str2);
                        strcpy(msg,"File deletion completed.\n");
                    }else{

                        /*
                        Appropriate error message to be returned to client
                        based on situation (ﬁle does not exist / client does not have access /
                        invalid line numbers)
                        */
                        if(checkFileName(str2)){
                            printf("File %s does not exist.\n",str2);
                            strcpy(msg,"File does not exist.\n");
                        }
                        else if(checkFilePermission(str2,clientsd,owner,editor_permission) == 0){
                            printf("Client does not have access for File %s .\n",str2);
                            strcpy(msg,"Client does not have access for File.\n");
                        }
                        else if( total_lines == 0){
                            printf("File %s is empty.\n",str2);
                            strcpy(msg,"File is empty.\n");
                        }
                        else if(start_idx > end_idx  || total_lines <= start_idx || total_lines <= end_idx || start_idx < 0  || end_idx < 0){
                            printf("invalid line numbers for File %s .\n",str2);
                            strcpy(msg,"invalid line numbers for File.\n");
                        }
                        else{
                            printf("Wrong Command.\n");
                            strcpy(msg,"Wrong Command.\n");
                        }
                    }

                }
                else if(strcmp(str1,"/invite")==0 && valid == 3)
                {
                    char extra[STR];
                    int valid = sscanf(str,"%s %s %d %s %[^\n]\n",str1,str2,&start_idx,str4,extra);
                    int invite_client_id,permission,soket_invite_client,edit_permission;
                    int owner_permission = 1;

                    if(strcmp(str4,"E")==0){
                        permission = 2;                              // 2 = editor , 3 = viewer
                    }
                    else if(strcmp(str4,"V")==0){
                        permission = 3;                             // 2 = editor , 3 = viewer
                    }
                    else{
                        permission = 0; 
                    }

                    invite_client_id = start_idx;

                    soket_invite_client = getClientSoket(client_details,invite_client_id);

                    if(permission == 2){
                        edit_permission = 1;
                    }
                    else{
                        edit_permission = 0;
                    }

                    if(checkFileName(str2) == 0 && (permission == 2 || permission == 3) && checkClientStatus(client_details,invite_client_id) == 1 && checkFilePermission(str2,clientsd,owner,owner_permission) == 1  && checkFilePermission(str2,soket_invite_client,invite_client_id,edit_permission) == 0 && valid == 4){

                        send(soket_invite_client ,str,MAX,0);
                        bzero(msg,sizeof(msg));

                        printf("Invitation sent to Client %d for File %s \n",invite_client_id,str2);
                        strcpy(msg,"Invitation sent to Client.\n");
                    }else{

                        if(checkFileName(str2) == 1 && valid == 4){
                            printf("File %s does not exist.\n",str2);
                            strcpy(msg,"File does not exist.\n");
                        }
                        else if(checkFilePermission(str2,clientsd,owner,owner_permission) == 0 && valid == 4){
                            printf("The Client %d is not the owner of the File %s.\n",owner,str2);
                            strcpy(msg,"You are not the owner of the File.\n");
                        }
                        else if(checkClientStatus(client_details,invite_client_id) == 0 && valid == 4){
                            printf("Invited Client %d does not exist.\n",invite_client_id);
                            strcpy(msg,"Invited Client does not exist.\n");
                        }
                        else if(checkFilePermission(str2,soket_invite_client,invite_client_id,edit_permission) == 1 && permission != 0 && valid == 4){
                            printf("The Client %d already has the permission.\n",invite_client_id);
                            strcpy(msg,"The Client already has the permission.\n");
                        }
                        else{
                            printf("Wrong command received.\n");
                            bzero(msg,sizeof(msg));
                            strcpy(msg,"Wrong command.\n");
                        }
                    }
                    
                }
                else if(strcmp(str1,"/response")==0 && valid == 2)
                {
                    int valid = sscanf(str,"%s %s %s %s",str1,str2,str3,str4);
                    int permission = 0,soket_invite_client;

                    if(strcmp(str3,"E")==0){
                        permission = 2;                              // 2 = editor , 3 = viewer
                    }
                    else if(strcmp(str3,"V")==0){
                        permission = 3;                             // 2 = editor , 3 = viewer
                    }

                    soket_invite_client = getFileOwnerFd(client_details,str2);
                    bzero(msg,sizeof(msg));

                    if(strcmp(str4,"Yes")==0){

                        updateFileCollaborator(str2,owner,permission);

                        printf("The client has accecpted the invitation.\n");
                        strcpy(msg,"The client has accecpted the invitation.\n");
                    }else{

                        printf("The client declined the invitation.\n");
                        strcpy(msg,"The client declined the invitation.\n");
                    }

                    send(soket_invite_client ,msg,MAX,0);

                    if(strcmp(str4,"Yes")==0){
                        strcpy(msg,"The invitation got accecpted.\n");
                    }else{
                        strcpy(msg,"The client got declined.\n");
                    }
                }
                else if(strcmp(str,"/profile")==0)
                {
                    printf("Wrong command received.\n");
                    bzero(msg,sizeof(msg));
                    sprintf(msg,"Profile Unique ID %d .\n",owner);
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
                send(clientsd ,msg,MAX,0);
                usleep(9000);
                bzero(msg,sizeof(msg));                     
            
            }
        }
    }
    
    return 0;
}


/*
/upload <filename>: Upload the local ﬁle ﬁlename to the server
*/
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

/*
/download <filename>: Download the server ﬁle ﬁlename to the client, if
given client has permission to access that ﬁle
*/
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
        send(clientSocket,buffer,MAX,0);
        usleep(9000);
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
    char buffer[MAX],permit[STR];
    int count=0,clientsd,addrlen,b,d;
    b = stored_file;
    struct sockaddr_in address;
    addrlen = sizeof(address);
    send(clientSocket,&b,sizeof(b),0);

    for (int i = 0; i < b; i++) 
    {
        sprintf(buffer,"File %s,OWNER %d ,LINES %d",fileRecord[i].file_name,fileRecord[i].owner,fileRecord[i].lines);
        send(clientSocket,buffer,MAX,0);
        bzero(buffer,sizeof(buffer));

        d = fileRecord[i].total_collaborators;
        send(clientSocket,&d,sizeof(d),0);

        for (int j = 0; j < d; j++) 
        {
            bzero(permit,sizeof(permit));

            if(fileRecord[i].collaborators[j].permission == 2){
                sprintf(permit,"EDITOR");
            }else if(fileRecord[i].collaborators[j].permission == 3){
                sprintf(permit,"VIEWER");
            }

            sprintf(buffer,"Collaborator %d ,Permission %s",fileRecord[i].collaborators[j].collaborator_id ,permit);
            send(clientSocket,buffer,MAX,0);
            bzero(buffer,sizeof(buffer));
        }
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
    fileRecord[stored_file].total_collaborators = 0;
    stored_file++;

    fclose(fp);
}

/*
The server should maintain a track of all ﬁles records
ﬁle/data structure .
*/
void updateFileLines(char filename[]){
    FILE *fp;
    int n=0,len,count,count1,j,size=0,b;
    char buffer[MAX];
    b=0;
    int d = stored_file;

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

    if(strlen(buffer) == 0 && n == 1){
        n = 0;
    }

    for (int i = 0; i < d; i++) 
    {
        if(strcmp(fileRecord[i].file_name,filename) == 0){
            fileRecord[i].lines = n;
        }
    }

    fclose(fp);
}

/*
If the invite is successful, the server should send
success messages to both C1 and C2, and maintain track of collaborators
(and their access privilege V/E) along with owners in the permission
records ﬁle.
*/
void updateFileCollaborator(char filename[],int collaborator_id,int permission){

    int d = stored_file;
    int total,update = 0;

    for (int i = 0; i < d; i++) 
    {
        if(strcmp(fileRecord[i].file_name,filename) == 0){

            total = fileRecord[i].total_collaborators;
            for (int j = 0; j < total; j++) 
            {
                if(fileRecord[i].collaborators[j].collaborator_id == collaborator_id){
                    fileRecord[i].collaborators[j].permission = permission;
                    update = 1;
                }
            }
            if(update == 1){
                break;
            }
            else if(update == 0){
                fileRecord[i].collaborators[total].collaborator_id = collaborator_id;
                fileRecord[i].collaborators[total].permission = permission;
                total++;
                fileRecord[i].total_collaborators = total;
            }
        }
    }
}

/*
check filename is duplicate / exits or not 
*/
int checkFileName(char filename[]){

    int b = stored_file;

    for (int i = 0; i < b; i++) 
    {
        if(strcmp(fileRecord[i].file_name,filename) == 0){
            return 0;
        }
    }

    return 1;

}

/*
get file total lines 
*/
int getFilelines(char filename[]){

    int b = stored_file;

    for (int i = 0; i < b; i++) 
    {
        if(strcmp(fileRecord[i].file_name,filename) == 0){
            return fileRecord[i].lines;
        }
    }

    return 0;
}

/*
get file owner soket fd 
*/
int getFileOwnerFd(struct clientRecord client_details[],char filename[]){

    int b = stored_file,owner_id = 0,socket_id = 0;

    for (int i = 0; i < b; i++) 
    {
        if(strcmp(fileRecord[i].file_name,filename) == 0){
            owner_id = fileRecord[i].owner;
            socket_id = getClientSoket(client_details,owner_id);
        }
    }

    return socket_id;
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
    bzero(buffer,sizeof(buffer));
    
    for (int i = 0; i < CLIENT; i++) 
    {
        if(client_details[i].status !=0){   
            clientsd = client_details[i].socket_id;
            getpeername(clientsd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
            sprintf(buffer,"Client %d, ip %s , port %d, status Active \n",client_details[i].unique_id , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
            send(clientSocket,buffer,MAX,0);
            bzero(buffer,sizeof(buffer));
        }
    }
    recv(clientSocket,&b,sizeof(b),0);
    bzero(buffer,sizeof(buffer));
}

/*
check  client is active or not with client UNIQUEID
*/
int checkClientStatus(struct clientRecord client_details[],int client_id){
    int status = 0;
    for (int i = 0; i < CLIENT; i++) 
    {
        if(client_details[i].unique_id == client_id){   
            status = client_details[i].status;
        }
    }
    return status;
}

/*
get  client Soket is active or not with client UNIQUEID
*/
int getClientSoket(struct clientRecord client_details[],int client_id){
    int socket_id = 0;
    for (int i = 0; i < CLIENT; i++) 
    {
        if(client_details[i].unique_id == client_id){   
            socket_id = client_details[i].socket_id;
        }
    }
    return socket_id;
}

/*
/read <filename> <start_idx> <end_idx>: Read from ﬁle ﬁlename
starting from line index start_idx to end_idx . If only one index is speciﬁed, read
that line. If none are speciﬁed, read the entire ﬁle.
*/
void readIndex(char filename[],int clientSocket,int start_idx,int end_idx){
    FILE *fp;
    int n=0;
    char buffer[MAX];
    int count=0,clientsd,addrlen,b;
    b = end_idx - start_idx + 1;
    struct sockaddr_in address;
    addrlen = sizeof(address);
    send(clientSocket,&b,sizeof(b),0);

    fp=fopen(filename,"r");

    if(!fp)
    {
        printf("Error in the file.\n");
        exit(1);
    }
    while(n <start_idx)
    {
        memset(buffer, 0x00, MAX); // clean buffer
        fscanf(fp, "%[^\n]\n",buffer); // read file *prefer using fscanf
        n++;
    }

    for (int i = start_idx; i <= end_idx; i++) 
    {
        memset(buffer, 0x00, MAX); // clean buffer
        fscanf(fp, "%[^\n]\n",buffer); // read file *prefer using fscanf
        send(clientSocket,buffer,MAX,0);
        bzero(buffer,sizeof(buffer));
    }

    recv(clientSocket,&b,sizeof(b),0);
    bzero(buffer,sizeof(buffer));

    fclose(fp);
}

/*
/insert <filename> <idx> “<message>”: Write message at the line
number speciﬁed by idx into ﬁle ﬁlename . If idx is not speciﬁed, insert at the end.
Quotes around the message to demarcate it from the other ﬁelds.
*/
void insertIndex(char filename[],int clientSocket,int idx,char message[]){
    FILE *fp,*fc;
    int n=0;
    char buffer[MAX];
    int count=0,clientsd,addrlen,b;
    b = idx;

    fp=fopen(filename,"r");
    fc=fopen("copy.txt","w");

    if(!fp)
    {
        printf("Error in the file.\n");
        exit(1);
    }

    if(!fc)
    {
        printf("Error in the file.\n");
        exit(1);
    }

    while( n <idx)
    {
        memset(buffer, 0x00, MAX); // clean buffer
        fscanf(fp, "%[^\n]\n",buffer); // read file *prefer using fscanf
        fprintf(fc, "%s\n",buffer);
        n++;
    }

    fprintf(fc, "%s\n",message);

    while(!feof(fp))
    {
        memset(buffer, 0x00, MAX); // clean buffer
        fscanf(fp, "%[^\n]\n",buffer); // read file *prefer using fscanf
        fprintf(fc, "%s\n",buffer);
    }

    
    bzero(buffer,sizeof(buffer));

    fclose(fp);
    fclose(fc);

    restore("copy.txt",filename);
    updateFileLines(filename);
}

/*
/delete <filename> <start_idx> <end_idx>: Delete lines starting
from line index start_idx to end_idx from ﬁle ﬁlename. If only one index is
speciﬁed, delete that line. If none are speciﬁed, delete the entire contents of the
ﬁle.
*/
void deleteIndex(char filename[],int clientSocket,int start_idx,int end_idx){
    FILE *fp,*fc;
    int n=0;
    char buffer[MAX];
    int count=0,b;
    b = end_idx -start_idx +1;
    
    fp=fopen(filename,"r");
    fc=fopen("copy.txt","w");

    if(!fp)
    {
        printf("Error in the file.\n");
        exit(1);
    }

    if(!fc)
    {
        printf("Error in the file.\n");
        exit(1);
    }

    while( n <start_idx)
    {
        memset(buffer, 0x00, MAX); // clean buffer
        fscanf(fp, "%[^\n]\n",buffer); // read file *prefer using fscanf
        fprintf(fc, "%s\n",buffer);
        bzero(buffer,sizeof(buffer));
        n++;
    }

    for (int i = start_idx; i <= end_idx; i++) 
    {
        memset(buffer, 0x00, MAX); // clean buffer
        fscanf(fp, "%[^\n]\n",buffer); // read file *prefer using fscanf
        bzero(buffer,sizeof(buffer));
    }

    while(!feof(fp))
    {
        memset(buffer, 0x00, MAX); // clean buffer
        fscanf(fp, "%[^\n]\n",buffer); // read file *prefer using fscanf
        fprintf(fc, "%s\n",buffer);
        bzero(buffer,sizeof(buffer));
    }

    bzero(buffer,sizeof(buffer));

    fclose(fp);
    fclose(fc);

    restore("copy.txt",filename);
    updateFileLines(filename);
}

/*
copy one file data to another file 
*/
void restore(char sourceFile[],char targetFile[]){
   char ch;
   FILE *source, *target;

   source = fopen(sourceFile, "r");

   if (source == NULL)
   {
      printf("Press any key to exit...\n");
      exit(EXIT_FAILURE);
   }

   target = fopen(targetFile, "w");

   if (target == NULL)
   {
      fclose(source);
      printf("Press any key to exit...\n");
      exit(EXIT_FAILURE);
   }

   while ((ch = fgetc(source)) != EOF)
      fputc(ch, target);                    //copying the file to another file


   fclose(source);
   fclose(target);
}

/*
the server generates a unique 5-digit ID
*/
int uniqueIdGenerator(int unique_number ,struct clientRecord client_details[]){

    for (int i = 0; i < allClientsNo; i++) 
    {
        if(unique_number == allCLients[i]){
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
take double quoted string with newline and retrive the information
*/
char *retriveMessage(char message[]){
    int msglen,i;
    char temp[10];
    char *conv_msg = malloc(STR);
    bzero(conv_msg,sizeof(conv_msg));  
    msglen = strlen(message);

    for( i= 1; i<msglen-1; i++){
        if(message[i] == 92 && message[i+1] == 'n'){
            strcat(conv_msg,"\n");
            i++;
        }
        else{
           bzero(temp,sizeof(temp));
           sprintf(temp,"%c",message[i]);
           strcat(conv_msg,temp);
        }
    }
    return conv_msg;
}

/*
Download the server ﬁle ﬁlename to the client, if
given client has permission to access that ﬁle
*/
int checkFilePermission(char filename[],int clientSocket,int owner,int editorPermission){

    int perm = 0;
    int b = stored_file;
    int ttl_clb = 0;
    
    for (int i = 0; i < b; i++) 
    {
        if(fileRecord[i].owner == owner && strcmp(fileRecord[i].file_name,filename) == 0){
            perm = 1;
            return perm;
        }else if(strcmp(fileRecord[i].file_name,filename) == 0){
            ttl_clb = fileRecord[i].total_collaborators;
            for (int j = 0; j < ttl_clb; j++) 
            {
                if(fileRecord[i].collaborators[j].collaborator_id == owner && ( editorPermission == 0 || (editorPermission == 1 && fileRecord[i].collaborators[j].permission == 2))){
                    perm = 1;
                    return perm;
                }
            }
        }
    }
    return perm;
}