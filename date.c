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
#include<time.h>
char* getNext(char **ptr);
int main(int argc, char const *argv[])
{
    bool uflag=false,rflag=false;
    char *temp=strdup(argv[1]);
    char *str=getNext(&temp);

    str=getNext(&temp);
    //check for flags

    if(str!=NULL){
        for(int i=0;i<2;i++){
            if(str==NULL){
                break;
            }
            if(strcmp(str,"-u")==0){
                uflag=true;
                str=getNext(&temp);
            }
            else if(strcmp(str,"-R")==0){
                str=getNext(&temp);
                rflag=true;
            }
            else if(str[0]=='-'){
                printf("Wrong flag entered \n");
                _exit(-1);
            }
        }
    }
    time_t t=time(NULL);
    struct tm *time=localtime(&t);
    char *format="%A %d %B %Y %X %p %Z";
    if(uflag){
        time=gmtime(&t);
    }
    
    char stime[256];
    strftime(stime,256,format,time);
    if(rflag){
        strftime(stime,256,"%a, %d %h %G %H:%M:%S %z",time);
    }
    printf("%s\n",stime);
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

