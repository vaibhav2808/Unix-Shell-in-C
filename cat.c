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
    bool nflag=false,bflag=false;
    char *temp=strdup(argv[1]);
    char *str=getNext(&temp);
    str=getNext(&temp);

    //check for flags
    for(int i=0;i<2;i++){
        if(str==NULL){
            break;
        }
        if(strcmp(str,"-n")==0){
            nflag=true;
            str=getNext(&temp);
        }
        else if(strcmp(str,"-b")==0){
            str=getNext(&temp);
            bflag=true;
        }
        else if(str[0]=='-'){
            printf("Wrong flag entered \n");
            _exit(-1);
        }
    }

    if(str==NULL){                                                      //no file after cat
        printf("No file specified \n");
    }

    int lineNumber=0;
    while(str!=NULL){
        //open file
        int fileDescriptor=open(str,O_RDONLY);
        if(fileDescriptor<0){
            printf("cat: %s:No such file or directory\n",str);
            str=getNext(&temp);
            continue;
        }

        //read file and output
        int i=0;
        char string[150];
        while(read(fileDescriptor,&string[i++],1)){
            if(string[i-1]=='\n'){
                string[i]='\0';                                          //remove newline character so it does not interfare with integer conversion
                i=0;
                lineNumber++;

                //output to stdout
                char snum[10];
                sprintf(snum,"%d",lineNumber);

                //handles flag
                if(nflag||bflag){
                    if(bflag&&strlen(string)==1){
                        lineNumber--;
                    }
                    else{
                        write(1,snum,strlen(snum));
                        write(1,"\t",1);
                    }
                }
                
                write(1,string,strlen(string));
            }
        }
        write(1,"\n",1);
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
