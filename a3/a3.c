#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<stdlib.h>
#include <sys/mman.h>

#define NAME_RESP "RESP_PIPE_36931" 
#define NAME_REQ "REQ_PIPE_36931"

typedef struct section{
    char sect_name[12];
    char sect_type;
    int sect_offset;
    int sect_size;
} section_header;

int main(int argc, char **argv){
    int fileD_read = -1; 
    int fileD_write = -1;

    if (mkfifo(NAME_RESP, 0666) == -1) {
        printf("ERROR\ncannot create the response pipe");
        return -1;
    } 
    
    fileD_read = open(NAME_REQ, O_RDONLY);
    if (fileD_read == -1) {
        printf("ERROR\ncannot open the request pipe\n");
        return -1;
    }

    fileD_write = open(NAME_RESP, O_WRONLY);
    if (fileD_write == -1) {
        perror("ERROR\ncannot open the response pipe\n");
        return -1;
    }

    char mesaj_con[] = "START";
    int len=strlen(mesaj_con);
    write(fileD_write, &len, 1);
    write(fileD_write, mesaj_con, len);
    printf("SUCCESS\n");

    for(;;){
        read(fileD_read,&len,1);
        char st[251];
        read(fileD_read,st,len);
        st[len]='\0';
        if(strcmp(st,"VARIANT")==0){
            char var[]="VARIANT";
            int varL = strlen(var);
            unsigned int nnr = 36931;
            char val[]="VALUE";
            int valL = strlen(val);
            write(fileD_write,&varL,1);
            write(fileD_write,var,varL);

            write(fileD_write,&nnr,4);

            write(fileD_write,&valL,1);
            write(fileD_write,val,valL);
        }
        else if(strcmp(st,"EXIT")==0){
            break;
        }
        else {
            break;
        }
    }

    close(fileD_write);
    close(fileD_read);
    unlink(NAME_RESP);
    return 0;
}
