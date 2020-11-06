#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<ctype.h>
#include<stdbool.h>
#include<errno.h>
#include<sys/stat.h>
char* getNext(char **ptr);
int main(int argc, char const *argv[])
{
    char *temp=strdup(argv[1]);
    char *freeMem=temp;
    char *str=getNext(&temp);
    str=getNext(&temp);
    bool printmessage=false;
    bool pathm=false;
    for(int i=0;i<2;i++){
        if(str==NULL){
                break;
        }
        if(strcmp(str,"-v")==0){
            printmessage=true;
            str=getNext(&temp);
        }
        else if(strcmp(str,"-p")==0){
            str=getNext(&temp);
            pathm=true;
        }
        else if(str[0]=='-'){
            printf("Wrong flag entered \n");
            _exit(-1);
        }
    }
    while(str!=NULL){

        //handling space in name
        char buf[256];
        while(str[strlen(str)-1]=='\\'){
            char *next=getNext(&temp);
            buf[0]='\0';
            strncpy(buf,str,strlen(str)-1);
            buf[strlen(str)-1]='\0';
            strcat(buf," ");
            strcat(buf,next);
            str=buf;
        }


        int err=mkdir(str,0777);
        printf("%s",str);
        if(err!=0){  
            if(errno==ENOENT){
                if(pathm){
                    //write code for flag handling
                    char *folders=strdup(str);
                    char *token=strsep(&folders,"/");
                    char path[200];
                    while(token!=NULL){
                        strcat(path,token);
                        mkdir(path,0777);
                        token=strsep(&folders,"/");
                        strcat(path,"/");
                    }
                }
                else{
                    printf("File path doesn't exist. If you want to create parent directory, use -p flag\n");
                    fflush(stdout);
                }
            }
            else{
                printf("Error: \n",strerror(errno));
                fflush(stdout);
                free(freeMem);
                _exit(-1);
            }
        }
        else{
            if(printmessage){
                printf("mkdir: created directory %s\n",str);
            }
        }
        str=getNext(&temp);
    }
    free(freeMem);
    _exit(0);
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