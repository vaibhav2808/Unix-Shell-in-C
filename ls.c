#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<ctype.h>
#include<stdbool.h>
#include<errno.h>
#include<sys/stat.h>
#include<dirent.h>
char* getNext(char **ptr);
int main(int argc, char const *argv[])
{
    bool iflag=false,bflag=false;
    char *temp=strdup(argv[1]);
    char *str=getNext(&temp);

    str=getNext(&temp);
    //check for flags

    if(str!=NULL){
        for(int i=0;i<2;i++){
            if(str==NULL){
                break;
            }
            if(strcmp(str,"-i")==0){
                iflag=true;
                str=getNext(&temp);
            }
            else if(strcmp(str,"-1")==0){
                str=getNext(&temp);
                bflag=true;
            }
            else if(str[0]=='-'){
                printf("Wrong flag entered \n");
                _exit(-1);
            }
        }
    }
    
    struct dirent **content;
    int n;
    if(str!=NULL){
        n=scandir(str,&content,NULL,alphasort);
    }
    else{
        n=scandir(".",&content,NULL,alphasort);
    }
    if(n==-1){
        printf("Error: %s \n",strerror(errno));
        _exit(-1);
    }

    
    char c[]="        ";
    if(bflag){
        c[0]='\n';
        c[1]='\0';
    }
        
    for(int i=0;i<n;i++){
        if(content[i]->d_name[0]=='.'){
            continue;
        }
        if(iflag)
            printf("%s %d %s",content[i]->d_name,content[i]->d_ino,c);
        else
            printf("%s%s",content[i]->d_name,c);
        
        free(content[i]);
    }
    free(content);
    printf("\n");

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
