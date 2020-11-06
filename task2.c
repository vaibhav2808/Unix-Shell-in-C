#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<ctype.h>
#include<sys/wait.h>
#include <fcntl.h> 
#include<errno.h>
#include<stdbool.h>

void removeTrailingSpaces(char *ptr,int len);
void handlePwd(char *temp,char *path);
void handleEcho(char *temp,char *path);
void handleHistory(char *temp,char *history);
void handleCd(char *temp,char *path);
void handleExternalCommand(char *temp,char *command,char *path);
char* getNext(char **ptr);
int isExternalCommand(char *command);
void readFile(char *file,char*path);

int main(){
    char* commands=NULL;
    char* history=(char *)malloc(10000*sizeof(char));
    char externalCommandPath[250];
    getcwd(externalCommandPath,250);
    size_t size;
    while(1){

        printf("%s:",getlogin());
        int slen=getline(&commands,&size,stdin);
        removeTrailingSpaces(commands,slen);
        
        if(slen==0){
            continue;
        }
        commands[slen-1]='\0';
        strcat(history,"\n");
        strcat(history,commands);
        
        char *temp=strdup(commands);
        char *first=strsep(&temp," ");
        if(strcmp(first,"exit")==0){
            _exit(0);
        }
        else if(strcmp(first,"pwd")==0){
            handlePwd(temp,externalCommandPath);
        }
        else if(strcmp(first,"echo")==0){
            handleEcho(temp,externalCommandPath);
        }
        else if(strcmp(first,"history")==0){
            handleHistory(temp,history);
        }
        else if(strcmp(first,"cd")==0){
            handleCd(temp,externalCommandPath);
        }
        else if(isExternalCommand(first)) {
            handleExternalCommand(commands,first,externalCommandPath);
        }
        else{
            printf("Enter a valid command\n");
        }
    }
    free(commands);
    free(history);
    return 0;
}

void handleExternalCommand(char *temp,char *command,char* path){
    pid_t pid;
    int status;
    pid=fork();
    if(pid<-1){
        printf("Error creating process\n");
    }
    else if(pid==0){
        strcat(path,"/");
        strcat(path,command);
        
        execl(path,path,temp,NULL);
    }
    else{
        waitpid(pid,&status,0);
        // if(status!=0){
        //     printf("error in fork process\n");
        // }
    }
}

void removeTrailingSpaces(char *ptr,int len){
    ptr+=len-2;
    while(isspace(*ptr)){
        ptr-=1;
    }
    *(ptr+1)='\0';
}

void handlePwd(char *temp,char *path){
    bool Lflag=false,Pflag=false;
    char *str=getNext(&temp);
     //check for flags
    for(int i=0;i<2;i++){
        if(str==NULL){
            break;
        }
        if(strcmp(str,"-L")==0){
            Lflag=true;
            str=getNext(&temp);
        }
        else if(strcmp(str,"-P")==0){
            str=getNext(&temp);
            Pflag=true;
        }
        else if(strcmp(str,"--help")==0){
            readFile("pwd-help.txt",path);
            return;
        }
        else if(str[0]=='-'){
            printf("Wrong flag entered \n");
            return;
        }
    }
    int err;
    char var[512];
    char *buf=var;
    if(getcwd(buf,512)==NULL){
        printf("Error: %s \n",strerror(errno));
        return;
    }
    if(Pflag){
        char tempbuf[256];
        int err2=readlink(str,buf,256);                      //returns size of buffer
        if(err2!=-1){                   //error
            buf[err2]='\0';
            if(err2!=0)              //no symbolic links exist
                str=buf;
        } 
    }
    
    printf("%s\n",buf);
}

void handleEcho(char *temp,char* path){                 //done
    char *str=getNext(&temp);
    char newline[]="\n";
    for(int i=0;i<2;i++){
        if(str==NULL){
            break;
        }
        if(strcmp(str,"-n")==0){
            *(newline+0)='\0';
            str=getNext(&temp);
        }
        else if(strcmp(str,"-E")==0){
            str=getNext(&temp);
        }
        else if(strcmp(str,"--help")==0){
            readFile("echo-help.txt",path);
            return;
        }
        else if(str[0]=='-'){
            printf("Wrong flag entered \n");
            return;
        }
    }
    while(str!=NULL){
        printf("%s ",str);
        str=getNext(&temp);
    }
    printf("%s",newline);
}

void handleHistory(char *temp,char *history){
    char *arr=strdup(history);
    char *str=strsep(&arr,"\n");    
    str=strsep(&arr,"\n");                    
    int i=1;
    bool cflag=false,wflag=false;
    char *str2=getNext(&temp);
    //handle flag
    for(int i=0;i<2;i++){
        if(str2==NULL){
            break;
        }
        if(strcmp(str2,"-c")==0){
            cflag=true;
            str2=getNext(&temp);
        }
        else if(strcmp(str2,"-w")==0){
            str2=getNext(&temp);
            wflag=true;
        }
        else if(str[0]=='-'){
            printf("Wrong flag entered \n");
            return;
        }
    }
    //flag haandling
    if(cflag){
        history[0]='\0';
        return;
    }

    if(wflag){
        mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
        int fd=open("history.txt",O_APPEND|O_CREAT|O_WRONLY,mode);
        if(fd<0){
            printf("Error while accessing history file \n");
            return;
        }
        int o=write(fd,history,strlen(history));
        if(o==-1){
             printf("Error writing: %s\n",strerror(errno));
        }
        int p=close(fd);
        if(p==-1){
             printf("Error closing file: %s\n",strerror(errno));
        }
        return;
    }
    
    while(str!=NULL){
        printf("%d %s\n",i++,str);
        str=strsep(&arr,"\n");
    }
}

void handleCd(char *temp,char *path){
    bool Lflag=false,Pflag=false,eflag=false;
    char *str=getNext(&temp);
    char buf[256];
     //check for flags
    for(int i=0;i<2;i++){
        if(str==NULL){
            break;
        }
        if(strcmp(str,"-L")==0){
            Lflag=true;
            str=getNext(&temp);
        }
        else if(strcmp(str,"-P")==0){
            str=getNext(&temp);
            Pflag=true;
        }
        else if(strcmp(str,"-e")==0){
            str=getNext(&temp);
            eflag=true;
        }
        else if(strcmp(str,"--help")==0){
            readFile("cd-help.txt",path);
            return;
        }
        else if(str[0]=='-'){
            printf("Wrong flag entered \n");
            return;
        }
    }
    int err;
    
    if(str==NULL){
        err=chdir(getenv("HOME"));
    }
    else{
        if(Pflag){
            char buf[256];
            int err2=readlink(str,buf,256);                      //returns size of buffer
            if(err2!=-1){                   //error
                buf[err2]='\0';
                if(err2!=0)              //no symbolic links exist
                    str=buf;
            }
        }
        if(eflag){
            char buf[100];
            if(getcwd(buf,100)==NULL){
                return;
            }
        }
        
        while(str[strlen(str)-1]=='\\'){
            char *next=getNext(&temp);
            buf[0]='\0';
            strncpy(buf,str,strlen(str)-1);
            buf[strlen(str)-1]='\0';
            strcat(buf," ");
            strcat(buf,next);
            str=buf;
        }
        err=chdir(str);
    }
    
    if(err!=0){
        printf("Error: %s \n",strerror(errno));
        return;
    }
}

char* getNext(char **ptr){
    char *temp=*ptr;
    if(temp==NULL){
        return NULL;
    }
    char *str=strsep(&temp, " ");
    while(str!=NULL&&strlen(str)==0){
        str=strsep(&temp," ");
    }
    *ptr=temp;
    return str;
}

int isExternalCommand(char *command){
    int v=0;
    if(strcmp(command,"date")==0||strcmp(command,"rm")==0||strcmp(command,"ls")==0||strcmp(command,"cat")==0||strcmp(command,"mkdir")==0){
        v=1;
    }
    return v;
}

void readFile(char *file,char*path){
    char str[250];
    str[0]='\0';
    strcat(str,path);
    strcat(str,"/");
    strcat(str,file);
    int fileDescriptor=open(str,O_RDONLY);

    if(fileDescriptor<0){
        printf("Error opeing help\n");
        return;
    }
    char c;
    while(read(fileDescriptor,&c,1)){
        printf("%c",c);
    }
    close(fileDescriptor);
}