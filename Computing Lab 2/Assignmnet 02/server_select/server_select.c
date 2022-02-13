/*
Author ---  Md Ashik Khan
ID --- 21CS60A02
task ---    CL-2 ASSIGNMENT
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
#include <regex.h>
#define CLIENT 5
#define MAX 1024
#define PORT 8080

struct transaction {
  int date;
  int month;
  int year;
  char item_name[256];
  float price;
} ;

/*
    this function use to check the daye , month and year of a  date,
    and returns the date is valid or not .
*/
int validDate(int d ,int m,int y){
    
    int daysinmonth[12]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int legit = 0;
    
        // leap year checking, if ok add 29 days to february
        if(y % 400 == 0 || (y % 100 != 0 && y % 4 == 0))
            daysinmonth[1]=29;

        // days in month checking
        if (m<13)
        {
            if( d <= daysinmonth[m-1] )
                legit=1;
        }

   return legit;
}

/*
    this function use to returns  the daye , month and year between two dates,
    and returns the greater date  .
*/
int compare_dates (int date1,int month1,int year1,int date2,int month2,int year2)
{
    if (year1 < year2)
       return -1;

    else if (year1 > year2)
       return 1;

    if (year1 == year2)
    {
         if (month1<month2)
              return -1;
         else if (month1>month2)
              return 1;
         else if (date1<date2)
              return -1;
         else if(date1>date2)
              return 1;
         else
              return -1;
    }
}

/*
    Before merge, this function  checks the file sent by the client is sorted or not.
    If not sorted , the merge function won't be executed.
*/
int checksorting (char filename[],char field[]){
    struct transaction *fileContent;
    FILE *fp;
    int len,b=0,n,l=0,i=0;
    int present_date,present_month,present_year;
    int next_date,next_month,next_year;
    float present_price,next_price;
    char present_item[800],next_item[800];
    char buffer[MAX],item_name[MAX],c;
    float price;
	fp=fopen(filename,"r");
    if(!fp)
    {
        printf("Error in the file.\n");
        exit(1);
    }
    bzero(buffer,sizeof(buffer));
    fseek(fp, 0, SEEK_SET); // make sure start from 0
    rewind(fp);
    for (c = getc(fp); c != EOF; c = getc(fp)){
        if (c == '\n') // Increment count if this character is newline
        {
            l++;
        }
    }
    fseek(fp, 0, SEEK_SET); // make sure start from 0
    rewind(fp);

    bzero(buffer,sizeof(buffer));
    bzero(next_item,sizeof(next_item));
    fscanf(fp, "%[^\n]\n",buffer);
    buffer[strlen(buffer) -1] ='\0';
    sscanf(buffer, "%d.%d.%d\t%[^\t]\t%f",&present_date,&present_month,&present_year,present_item,&present_price);

    for(i=0;i<l-1;i++){
        bzero(buffer,sizeof(buffer));
        bzero(next_item,sizeof(next_item));
        fscanf(fp, "%[^\n]\n",buffer);
        buffer[strlen(buffer) -1] ='\0';
		sscanf(buffer, "%d.%d.%d\t%[^\t]\t%f",&next_date,&next_month,&next_year,next_item,&next_price);
        if (strcmp(field,"price") == 0  && present_price > next_price) {
            return 0;
        }
        else if(strcmp(field,"date") == 0 && compare_dates( present_date,present_month,present_year,next_date,next_month,next_year) == -11){
           return 0;
        }
        else if (strcmp(field,"item_name") == 0  && strcmp(present_item, next_item) > 0) {
            return 0;
        }
        present_date = next_date;
        present_month = next_month;
        present_year = next_year;
        strcpy(present_item,next_item);
        present_price = next_price;
	}

    return 1;
}

/*
    Merges two subarrays of arr[].
    First subarray is arr[l..m]
    Second subarray is arr[m+1..r]
*/        
void merge(struct transaction arr[], int l, int m, int r,char field[])
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
  
    struct transaction L[n1], R[n2];
  
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];
  
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2) {
        if (strcmp(field,"price") == 0  && L[i].price <= R[j].price) {
            arr[k] = L[i];
            i++;
        }
        else if(strcmp(field,"date") == 0 && compare_dates(L[i].date,L[i].month,L[i].year, R[j].date,R[j].month,R[j].year) == -1){
            arr[k] = L[i];
            i++;
        }
        else if (strcmp(field,"item_name") == 0  && strcmp( R[j].item_name ,L[i].item_name) > 0) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
  
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
  
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

/* 
l is for left index and r is right index of the
sub-array of arr to be sorted 
*/
void mergeSort(struct transaction arr[], int l, int r,char field[])
{
    if (l < r) {
        int m = l + (r - l) / 2;
  
        mergeSort(arr, l, m,field);
        mergeSort(arr, m + 1, r,field);
  
        merge(arr, l, m, r,field);
    }
}

/* 
Before sort/ merge, this function  checks the file sent by the client is in right formatted or not.
If not valid formatted , the sort/merge function won't be executed.
*/
int validateFile(char filename[]){
	regex_t fileValidateregex;
    int n;
    int d,m,y;
	FILE *fp;
	char buffer[MAX],item_name[MAX];
    float price;
    fp=fopen(filename,"r");
	while(!feof(fp))
    {
        bzero(buffer,sizeof(buffer));
        bzero(item_name,sizeof(item_name));
        fscanf(fp, "%[^\n]\n",buffer);
        buffer[strlen(buffer) -1] ='\0';
		n = sscanf(buffer, "%d.%d.%d\t%[^\t]\t%f",&d,&m,&y,item_name,&price);
		// printf("%d . %d . %d \t %s \t %.2f\n",d,m,y,item_name,price);
		if( n != 5){
			return 0;
		}else if(validDate(d,m,y) == 0 ){
            return 0;
        }
	}
	return 1;
}

/*
this function used to sort the downloaded file from client using mergesort.
For, this , first calcluate the file lines, check any empty line exits or not.
If empty line exits, substract from file lines and then sort using mergesort function.
*/
void sortFile(char filename[],char field[]){
   
    struct transaction *fileContent;
    FILE *fp;
    int len,b=0,n,l=0,i=0,empt=0;
    int d,m,y;
    char buffer[MAX],item_name[MAX],c;
    float price;
	fp=fopen(filename,"r");
    if(!fp)
    {
        printf("Error in the file.\n");
        exit(1);
    }
    bzero(buffer,sizeof(buffer));
    fseek(fp, 0, SEEK_SET); // make sure start from 0
    rewind(fp);
    for (c = getc(fp); c != EOF; c = getc(fp)){
        if (c == '\n') // Increment count if this character is newline
        {
            l++;
        }
    }
    fseek(fp, 0, SEEK_SET); // make sure start from 0
    rewind(fp);
    for(i=0;i<l;i++){
        bzero(buffer,sizeof(buffer));
        fscanf(fp, "%[^\n]\n",buffer);
        if(strcmp(buffer,"\0") == 0){
            empt++;
        }
	}
    l = l -empt;
    fileContent=(struct transaction *)malloc(l* sizeof(struct transaction));
    fseek(fp, 0, SEEK_SET); // make sure start from 0
    rewind(fp);
    for(i=0;i<l;i++){
        bzero(buffer,sizeof(buffer));
        fscanf(fp, "%[^\n]\n",buffer);
        buffer[strlen(buffer) -1] ='\0';
		n = sscanf(buffer, "%d.%d.%d\t%[^\t]\t%f",&fileContent[i].date,&fileContent[i].month,&fileContent[i].year,fileContent[i].item_name,&fileContent[i].price);
        // printf("%d.%d.%d\t%s\t%f\n",fileContent[i].date,fileContent[i].month,fileContent[i].year,fileContent[i].item_name,fileContent[i].price);
	}
    
    bzero(buffer,sizeof(buffer));
    fclose(fp);

    fp=fopen(filename,"w");
    if(!fp)
    {
        printf("Error in the file.\n");
        exit(1);
    }
    mergeSort(fileContent,0,l-1,field);
    for(i=0;i<l;i++){
		fprintf(fp, "%d.%d.%d\t%s\t%.2f\n",fileContent[i].date,fileContent[i].month,fileContent[i].year,fileContent[i].item_name,fileContent[i].price);
		// printf("%d.%d.%d\t%s\t%.2f\t|\t\n",fileContent[i].date,fileContent[i].month,fileContent[i].year,fileContent[i].item_name,fileContent[i].price);
	}
    fclose(fp);
    printf("File has been sorted.\n");
}

/*
this function used to merge the downloaded two files from client and store the datas into another file.
For, this , first calcluate the file lines of two files, check any empty line exits or not.
If empty line exits, substract from file lines and then store the values into a custom structed type array .
then sort using mergesort function.
Again, store the data into  client inputed file name.
*/
void mergeFile(char filename_1[],char filename_2[],char filename_3[],char field[]){
   
    struct transaction *fileContent;
    FILE *fp1,*fp2,*fpd;
    int len,b=0,n,m=0,l=0,i=0;
    // int d,m,y;
    char buffer[MAX],item_name[MAX],c;
    float price;
	fp1=fopen(filename_1,"r");
	fp2=fopen(filename_2,"r");
    if(!fp1 || !fp2)
    {
        printf("Error in the file.\n");
        exit(1);
    }
    bzero(buffer,sizeof(buffer));
    fseek(fp1, 0, SEEK_SET); // make sure start from 0
    rewind(fp1);
    for (c = getc(fp1); c != EOF; c = getc(fp1)){
        if (c == '\n') // Increment count if this character is newline
        {
            m++;
        }
    }
    fseek(fp2, 0, SEEK_SET); // make sure start from 0
    rewind(fp2);
    for (c = getc(fp2); c != EOF; c = getc(fp2)){
        if (c == '\n') // Increment count if this character is newline
        {
            l++;
        }
    }
    fileContent=(struct transaction *)malloc((l+m)* sizeof(struct transaction));
    fseek(fp1, 0, SEEK_SET); // make sure start from 0
    rewind(fp1);
    for(i=0;i<m;i++){
        bzero(buffer,sizeof(buffer));
        fscanf(fp1, "%[^\n]\n",buffer);
        buffer[strlen(buffer) -1] ='\0';
		n = sscanf(buffer, "%d.%d.%d\t%[^\t]\t%f",&fileContent[i].date,&fileContent[i].month,&fileContent[i].year,fileContent[i].item_name,&fileContent[i].price);
        // printf("%d.%d.%d\t%s\t%f\n",fileContent[i].date,fileContent[i].month,fileContent[i].year,fileContent[i].item_name,fileContent[i].price);
	}
    
    bzero(buffer,sizeof(buffer));
    fclose(fp1);
    fseek(fp2, 0, SEEK_SET); // make sure start from 0
    rewind(fp2);
    for(i=m;i<(l+m);i++){
        bzero(buffer,sizeof(buffer));
        fscanf(fp2, "%[^\n]\n",buffer);
        buffer[strlen(buffer) -1] ='\0';
		n = sscanf(buffer, "%d.%d.%d\t%[^\t]\t%f",&fileContent[i].date,&fileContent[i].month,&fileContent[i].year,fileContent[i].item_name,&fileContent[i].price);
        // printf("%d.%d.%d\t%s\t%f\n",fileContent[i].date,fileContent[i].month,fileContent[i].year,fileContent[i].item_name,fileContent[i].price);
	}
    
    bzero(buffer,sizeof(buffer));
    fclose(fp2);

    fpd=fopen(filename_3,"w");
    if(!fpd)
    {
        printf("Error in the file.\n");
        exit(1);
    }
    mergeSort(fileContent,0,(l+m-1),field);
    for(i=0;i<(l+m);i++){
		fprintf(fpd, "%d.%d.%d\t%s\t%.2f\t|\t\n",fileContent[i].date,fileContent[i].month,fileContent[i].year,fileContent[i].item_name,fileContent[i].price);
		// printf("%d.%d.%d\t%s\t%.2f\t|\t\n",fileContent[i].date,fileContent[i].month,fileContent[i].year,fileContent[i].item_name,fileContent[i].price);
	}
    fclose(fpd);
    printf("File has been sorted.\n");
}

/*
This function used to find all pairs of similar
records in the two bills stored at filename1 and filename2.
And print the outputs in client terminal.
*/
void similarityFile(char filename_1[],char filename_2[],int clientSocket){
    struct transaction *fileContent1,*fileContent2,*similarContent,*otherContent;;
    FILE *fp1,*fp2,*fpd;
    int len,b=0,n,m=0,l=0,i=0,j=0,count =0,x,k=0;
    // int d,m,y;
    char buffer[1024],item_name[MAX],c;
    float price;
	fp1=fopen(filename_1,"r");
	fp2=fopen(filename_2,"r");
    if(!fp1 || !fp2)
    {
        printf("Error in the file.\n");
        exit(1);
    }
    bzero(buffer,sizeof(buffer));
    fseek(fp1, 0, SEEK_SET); // make sure start from 0
    rewind(fp1);
    for (c = getc(fp1); c != EOF; c = getc(fp1)){
        if (c == '\n') // Increment count if this character is newline
        {
            m++;
        }
    }
    fseek(fp2, 0, SEEK_SET); // make sure start from 0
    rewind(fp2);
    for (c = getc(fp2); c != EOF; c = getc(fp2)){
        if (c == '\n') // Increment count if this character is newline
        {
            l++;
        }
    }
    fileContent1=(struct transaction *)malloc((m)* sizeof(struct transaction));
    fseek(fp1, 0, SEEK_SET); // make sure start from 0
    rewind(fp1);
    for(i=0;i<m;i++){
        bzero(buffer,sizeof(buffer));
        fscanf(fp1, "%[^\n]\n",buffer);
        buffer[strlen(buffer) -1] ='\0';
		n = sscanf(buffer, "%d.%d.%d\t%[^\t]\t%f",&fileContent1[i].date,&fileContent1[i].month,&fileContent1[i].year,fileContent1[i].item_name,&fileContent1[i].price);
        // printf("%d.%d.%d\t%s\t%f\n",fileContent1[i].date,fileContent1[i].month,fileContent1[i].year,fileContent1[i].item_name,fileContent1[i].price);
	}
    
    fileContent2=(struct transaction *)malloc((l)* sizeof(struct transaction));
    fclose(fp1);
    fseek(fp2, 0, SEEK_SET); // make sure start from 0
    rewind(fp2);
    for(i=0;i<l;i++){
        bzero(buffer,sizeof(buffer));
        fscanf(fp2, "%[^\n]\n",buffer);
        buffer[strlen(buffer) -1] ='\0';
		n = sscanf(buffer, "%d.%d.%d\t%[^\t]\t%f",&fileContent2[i].date,&fileContent2[i].month,&fileContent2[i].year,fileContent2[i].item_name,&fileContent2[i].price);
        // printf("%d.%d.%d\t%s\t%f\n",fileContent[i].date,fileContent[i].month,fileContent[i].year,fileContent[i].item_name,fileContent[i].price);
	}
    
    bzero(buffer,sizeof(buffer));
    fclose(fp2);

    
    similarContent = (struct transaction *)malloc((m+l)* sizeof(struct transaction));
    otherContent = (struct transaction *)malloc((m+l)* sizeof(struct transaction));
    
    k = 0;
    for(i = 0; i < m ;i++) {
 
      for(j = 0; j < l ;j++) {
        if((fileContent1[i].date ==fileContent2[j].date && fileContent1[i].month ==fileContent2[j].month && fileContent1[i].year ==fileContent2[j].year)||fileContent1[i].price == fileContent2[j].price || strcmp(fileContent1[i].item_name,fileContent2[j].item_name) == 0){
            similarContent[k] = fileContent1[i] ;
            otherContent[k] = fileContent2[j] ;
            k++;
          }
       }
    } 

    bzero(buffer,sizeof(buffer));
    sprintf(buffer,"similar %d\n",k);
    send(clientSocket ,buffer,strlen(buffer),0);

    bzero(buffer,sizeof(buffer));
    for(i=0; i<k; i++)
    {       
            sprintf(buffer,"%d.%d.%d\t%s\t%.2f\t|\t%d.%d.%d\t%s\t%.2f\n",similarContent[i].date,similarContent[i].month,similarContent[i].year,similarContent[i].item_name,similarContent[i].price,otherContent[i].date,otherContent[i].month,otherContent[i].year,otherContent[i].item_name,otherContent[i].price);
            send(clientSocket,buffer,MAX,0);
            bzero(buffer,sizeof(buffer));
    }
    
}

/*
this function calculate how much char the file have and send this to clinet.
then clinet prepare for receiving the data. 
then send every char to client. 
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

/*
this function receive how much char the file have and prepare for receiving the data. 
then open the file and store the datas.
*/
void uploadFIle(char filename[],int clientSocket){
			int len,b=0;
            FILE *fp;
            char buffer[MAX],msg[MAX];
            sprintf(msg,"download %s\n",filename);
            send(clientSocket ,msg,strlen(msg),0);
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
            printf("Uploading  %s ......\n",filename);
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

int main(int argc , char *argv[])
{
    int opt=1,n,i,j,count=0,count1=0,len,b;
    int sock_fd , addrlen ,newsock_fd, client_socket[CLIENT],connected_client = 0 , activity , valread ,sd1;
    int max_sd,size,str_count;
    char msg[1024],buffer[1024];
    char str1[MAX],str2[MAX],str3[MAX],str4[MAX],str5[MAX],str[MAX];
    struct sockaddr_in address;
    fd_set readfds;

    //validation 
    regex_t fileregex;
    int comp = regcomp(&fileregex, "^\\w+.(txt)$", REG_EXTENDED | REG_NOSUB) ;
    

    //Client Handling
    for (i = 0; i < CLIENT; i++) 
    {
        client_socket[i] = 0;
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
    printf("Waiting for connections ...\n");
    while(1) 
    {
        FD_ZERO(&readfds);
        FD_SET(sock_fd, &readfds);
        max_sd =sock_fd;
        bzero(msg,sizeof(buffer));
        for ( i = 0 ; i < CLIENT ; i++) 
        {
            sd1 = client_socket[i];
            if(sd1 > 0)
                FD_SET( sd1 , &readfds);
            if(sd1 > max_sd)
                max_sd = sd1;
        }
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
   
        if ((activity < 0) && (errno!=EINTR)) 
        {
            printf("select error");
        }
        if (FD_ISSET(sock_fd, &readfds) ) 
        {
            if ((newsock_fd = accept(sock_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            if( connected_client < CLIENT ){
                printf("New connection , socket fd is %d , ip is : %s , port : %d \n" ,newsock_fd, inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                for (i = 0; i < CLIENT; i++) 
                {
                    if( client_socket[i] == 0 )
                    {
                        client_socket[i] =newsock_fd;
                        printf("Adding to list of sockets as %d\n" , i);
                        connected_client++;

                        strcpy(msg,"[+]Connected to Server\n\nUser Command:\n/sort <filename> <field>\n/merge <filename1> <filename2> <filename3> <field>\n/similarity <filename1> <filename2>\n\n/exit to exit from server.\n");
                        send(newsock_fd ,msg,strlen(msg),0);
                        bzero(msg,sizeof(buffer));
                        break;
                    }
                }
            }else{
                strcpy(msg,"limit_exceed");
                send(newsock_fd ,msg,strlen(msg),0);
                bzero(msg,sizeof(buffer)); 
            }
        }

        for (i = 0; i < CLIENT; i++) 
        {

            sd1 = client_socket[i];
             
            if (FD_ISSET( sd1 , &readfds)) 
            {
                bzero(str,sizeof(str));
                bzero(str1,sizeof(str1));
                bzero(str2,sizeof(str2));
                bzero(str3,sizeof(str3));
                bzero(str4,sizeof(str4));
                bzero(str5,sizeof(str5));
                if ((valread = read( sd1 ,str, MAX)) == 0)
                {
                    getpeername(sd1 , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                    close(sd1);
                    client_socket[i] = 0;
                    break;
                }
                printf("%s command received from client.\n",str);
                
                str_count = sscanf(str,"%s %s %s %s %s",str1,str2,str3,str4,str5);

                if(strcmp(str1,"/sort")==0 && !regexec(&fileregex, str2, 0 , 0 , 0) && (strcmp(str3,"date") == 0 || strcmp(str3,"item_name") == 0 || strcmp(str3,"price") == 0 ) && str_count == 3)
                {
                    downloadFIle(str2,sd1);
                    if(validateFile(str2) == 0 ){
                        printf("File document format is not valid.\n");
                        strcpy(msg,"Invalid file document format.\n");
                    }else{
                        bzero(msg,sizeof(buffer)); 
                        sortFile(str2,str3);
                        uploadFIle(str2,sd1);
                        bzero(msg,sizeof(buffer)); 
                        strcpy(msg,"Sorted Sucessfully.\n");
                    }

                }
                else if(strcmp(str1,"/merge")==0 && !regexec(&fileregex, str2, 0 , 0 , 0) && !regexec(&fileregex, str3, 0 , 0 , 0) && !regexec(&fileregex, str4, 0 , 0 , 0) && (strcmp(str5,"date") == 0 || strcmp(str5,"item_name") == 0 || strcmp(str5,"price") == 0 ))
                {
                    downloadFIle(str2,sd1);
                    downloadFIle(str3,sd1);
                    if(validateFile(str2) == 0 || validateFile(str3) == 0 ){
                        printf("File document format is not valid.\n");
                        strcpy(msg,"Invalid file document format.\n");
                    }
                    else if(checksorting(str2,str5) == 0 || checksorting(str3,str5) == 0){
                        printf("File documents order by %s  is not sorted .\n",str5);
                        strcpy(msg,"File documents are not sorted .\n");
                    }
                    else{
                        bzero(msg,sizeof(buffer)); 
                        mergeFile(str2,str3,str4,str5);
                        uploadFIle(str4,sd1);
                        bzero(msg,sizeof(buffer)); 
                        strcpy(msg,"Merged Sucessfully.\n");
                    }
                }
                else if(strcmp(str1,"/similarity")==0 && !regexec(&fileregex, str2, 0 , 0 , 0) && !regexec(&fileregex, str3, 0 , 0 , 0) && str_count == 3)
                {
                    
                    downloadFIle(str2,sd1);
                    downloadFIle(str3,sd1);
                    if(validateFile(str2) == 0 || validateFile(str3) == 0 ){
                        printf("File document format is not valid.\n");
                        strcpy(msg,"Invalid file document format.\n");
                    }else{
                        bzero(msg,sizeof(buffer)); 
                        similarityFile(str2,str3,sd1);
                        bzero(msg,sizeof(buffer)); 
                        strcpy(msg,"Operated Sucessfully.\n");
                    }
                }
                else if(strcmp(str,"/exit")==0)
                {
                    printf("Client disconnected.\n");
                    connected_client--;
                    bzero(msg,sizeof(buffer));
                    strcpy(msg,"Disconnected from server.\n");
                }
                else
                {
                    printf("Wrong command received.\n");
                    bzero(msg,sizeof(buffer));
                    strcpy(msg,"Wrong command.\n");
                }
                send(sd1 ,msg,strlen(msg),0);
                bzero(msg,sizeof(buffer));                     
                bzero(str,sizeof(str));                     
            
            }
        }
    }
    
    return 0;
}