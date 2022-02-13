/*	Assignment no.1
    SERVER SIDE
	Author : Md. Ashik Khan
	Roll-no :21CS60A02

    To compile the server program gcc server.c -o  server -lm
    To run server program ./server
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<math.h>
#include<ctype.h>
#define BUFFER 1000
#define PORT 8090 
#define SA struct sockaddr 

int nlinex();
int match(char str1[]);
void restore();
void printx();
void insertex(int line,char str[]);

void func(int sockfd) 
{ 
    FILE *fp;
    char str1[BUFFER];
    char str2[BUFFER];
    char str3[BUFFER];
    char message[BUFFER],c;
    int i,n,n1,n2,count,count1,count2,temp,line,result,k,des,insert;
    int opt;
    char str[1024]={0};
    char msg[BUFFER]; 
   
    for (;;) { 
        count=0,count1=0,count2=0,temp=0,insert=0;
        opt=1;
        bzero(str, BUFFER); 
        read(sockfd, str, sizeof(str));  
        printf("From client: %s\t To client : ", str); 

        //exit 
        if (strncmp("exit",str, 4) == 0) { 
            printf("Server Exit...\n");
            write(sockfd,str, sizeof(str)); 
            break; 
        }

        /* Extracting the command */
        bzero(str1, BUFFER); 
        bzero(str2, BUFFER); 
        bzero(str3, BUFFER); 

        n1 = sscanf(str,"%s %d %[^\n]",str1,&des,str3);
        if(n1 == 1){
            sscanf(str,"%s %[^\n]",str1,str2);
            insert = 1;
        }
    
        fp=fopen("server_file.txt","r");   //open the file in the read mode
        if(!fp)
        {
            printf("Error.\n");
            return;
        }
        result=match(str1);
        
        if(result==1 && n1 == 1)
        {
            line = nlinex();    
            printf("%d \n",line);
            sprintf(msg,"%d",line);
        }
        else if(result==2 && n1 != 3)
        {   
            if(n1 == 1){ 
                k=0;
            }else if(n1 == 2){
                k = des;
            }
            line = nlinex();

            if( k >= line || k < (line*-1) || (n1 == 1 && strlen(str2) > 0)){    // only value between 
                printf("Index is invalid.\n");
                strcpy(msg,"Index is invalid.\n");

            }else{
                if(k < 0 ){
                    k += nlinex();
                }
                k++;
                rewind (fp);
                for (int i = 0; i < k; i++)     
                {
                    // fscanf(fp, "%d\t%[^\n]\n", &line,message); // read the line *prefer using fscanf
                    fscanf(fp, "%[^\n]\n", message); // read the line *prefer using fscanf
                }
                printf("%s \n",message);
                sprintf(msg,"%s",message);
            }
        }
        else if(result==3 && (n1 == 3 || insert == 1))
        {
            rewind (fp);
            line = nlinex();
            
            if(n1 == 3){
                if(des < 0 ){
                    des += line;
                }
            }else if(insert == 1){
                des = line ;
                strcpy(str3,str2);
            } 

            if(strlen(str3) == 0){                                      // Check message is null or not 
                printf("Invalid Input Format.\n");
                strcpy(msg,"Invalid Input Format.\n");
            }
            else if( (des > line && insert == 0) || des < 0){     // read the line *prefer using fscanf
                printf("Index is invalid..\n");
                strcpy(msg,"Index is invalid.\n");
            }else if( des >= 0 || ( des < line && insert == 0) || (des == line && insert == 1)){
                insertex(des,str3);
                printf("Successfully inserted at Index %d  !!!\n",des);
                sprintf(msg,"Successfully inserted at Index %d  !!!",des);
            }
        }else if (strncmp("PRINTX",str, 4) == 0) {                  //Print the file
            printx();
            strcpy(msg,"Executed \n");
        }
        else{
            printf("Wrong choice \n");                              //Error Handling
            strcpy(msg,"Wrong choice ! \n Available Command: \n NLINEX \n READX \n READX <k> \n INSERTX <message> \n INSERTX <k> <message> \n");
        }
            
        bzero(str, BUFFER); 
        write(sockfd,msg, sizeof(msg));                             //Sending response to CLient
        
    } 
} 

int main() 
{ 
    int sockfd, connfd, len; 
    struct sockaddr_in servaddr, cli;  
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n"); 
    if ((listen(sockfd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    len = sizeof(cli); 
    connfd = accept(sockfd, (SA*)&cli, (socklen_t*)&len); 
    if (connfd < 0) { 
        printf("server acccept failed...\n"); 
        exit(0); 
    } 
    else
        printf("server acccept the client...\n"); 
    func(connfd); 
    close(sockfd); 
}

int match(char str[])
{
    char command[100];
    char str1[100]="NLINEX";
    char str2[100]="READX";
    char str3[100]="INSERTX";
    int n=strlen(str),count=0;
    
    if(strcmp(str1,str)==0)
        return 1;
    else if(strcmp(str2,str)==0)
        return 2;
    else if(strcmp(str3,str)==0)
        return 3;
    else
        return -1;

}

int nlinex(){
    FILE *fp;
    int line;
    char message[BUFFER];
    fp=fopen("server_file.txt","r");   //open the file in the read mode
    if(!fp)
    {
        printf("Press any key to exit...\n");
        exit(EXIT_FAILURE);
    }
    fseek(fp, 0, SEEK_SET); // make sure start from 0
    while(!feof(fp))
    {
        memset(message, 0x00, BUFFER); // clean buffer
        fscanf(fp, "%d\t%[^\n]\n", &line, message); // read file *prefer using fscanf
    }
    fclose(fp);
    return line;
}

void restore(){
   char ch;
   FILE *source, *target;

   source = fopen("copy_server_file.txt", "r");

   if (source == NULL)
   {
      printf("Press any key to exit...\n");
      exit(EXIT_FAILURE);
   }

   target = fopen("server_file.txt", "w");

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


void insertex(int k,char str[]){
   char ch,message[BUFFER];
   int i,line,limit;
   FILE *source, *target;

   limit = nlinex();

   char *insertxString = str;
   insertxString = str + 0;


   source = fopen("server_file.txt", "r");
   if (source == NULL)
   {
      printf("Press any key to exit...\n");
      exit(EXIT_FAILURE);
   }

   target = fopen("copy_server_file.txt", "w");
   if (target == NULL)
   {
      fclose(source);
      printf("Press any key to exit...\n");
      exit(EXIT_FAILURE);
   }

    for (int i = 0; i <= limit ; i++)
    {
        if(i == k){
            fprintf(target, "%d\t%s\n",i+1,insertxString);     //Setting the message to the targeted index
        }else if(i > k){
            fscanf(source, "%d %[^\n]\n", &line,message);
            fprintf(target, "%d\t%s\n", i+1,message);      //Increasing rest of the indexs
        }else{
            fscanf(source, "%d %[^\n]\n", &line,message);
            fprintf(target, "%d\t%s\n", line,message); 
        }
    }

    fclose(source);
    fclose(target);

    restore();
}

void printx(){
    FILE *fp;
    int line;
    char message[BUFFER];
    fp=fopen("server_file.txt","r");   //open the file in the read mode
    if(!fp)
    {
        printf("Press any key to exit...\n");
        exit(EXIT_FAILURE);
    }
    fseek(fp, 0, SEEK_SET); // make sure start from 0
    while(!feof(fp))
    {
        memset(message, 0x00, BUFFER); // clean buffer
        fscanf(fp, "%d\t%[^\n]\n", &line, message); // read file *prefer using fscanf
        printf("%d\t%s\n",line, message);
    }
    fclose(fp);
}








 