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
char* getNext(char **ptr);

int main(int argc, char const *argv[])
{   
    bool iflag=false,vflag=false;
    char *temp=strdup(argv[1]);
    char *str=getNext(&temp);
    str=getNext(&temp);

    //check for flags
    for(int i=0;i<2;i++){
        if(str==NULL){
            break;
        }
        if(strcmp(str,"-i")==0){
            iflag=true;
            str=getNext(&temp);
        }
        else if(strcmp(str,"-v")==0){
            str=getNext(&temp);
            vflag=true;
        }
        else if(str[0]=='-'){
            printf("Wrong flag entered \n");
            _exit(-1);
        }
    }
    //check if exist
    while(str!=NULL){
        if(iflag&&access(str,F_OK)!=0){
            printf("Error removing %s: %s \n",str,strerror(errno));
            _exit(-1);
        }

        //flag handle
        if(iflag){
            char prompt[10];
            printf("rm: remove file %s ",str);
            scanf("%s",prompt);
            if(strcmp(prompt,"n")==0){
                continue;
            }
        }
        int status=unlink(str);
        if(status==-1){
            printf("Error: %s \n",strerror(errno));
        }
        if(vflag&&status==0){
            printf("removed %s\n",str);
        }
        str=getNext(&temp);
    }
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
