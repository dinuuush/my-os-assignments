#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<stdlib.h>

typedef struct section{
    char sect_name[12];
    char sect_type;
    int sect_offset;
    int sect_size;
} section_header;

int listDir(char *path, int has_perm_execute,int greater_than_size ,int given_size){
    DIR *dir=NULL;
    struct dirent *entry=NULL;
    dir=opendir(path);
    if(dir==NULL){
        perror("ERROR\ninvalid directory path");
        return -1;
    }
    printf("SUCCESS");

    while((entry=readdir(dir))!=NULL){
        if((strcmp(entry->d_name,".")!=0 )&& (strcmp(entry->d_name,"..")!=0 )){
            char ppath[500];
            struct stat filestat;
            snprintf(ppath,500,"%s/%s",path,entry->d_name);
            if (stat(ppath, &filestat) == -1) {
                continue;
            }
            if(greater_than_size==1){
                if(S_ISREG(filestat.st_mode) && filestat.st_size>given_size){
                    printf("\n%s", ppath);
                }
            }
            else if(has_perm_execute==1){
                if ((filestat.st_mode & S_IXUSR) == S_IXUSR) {
                    printf("\n%s", ppath);
                }
            }
            else{
                printf("\n%s", ppath);
            }
        }
    }
    closedir(dir);
    return 0;
}

void listDirRec(char *path, int has_perm_execute,int greater_than_size, int given_size, int iteratie){
    DIR *dir=NULL;
    struct dirent *entry=NULL;
    dir=opendir(path);
    if(dir==NULL){
        if(iteratie==1)
        perror("ERROR\ninvalid directory path");
        return;
    }
    if(iteratie==1)
        printf("SUCCESS");
    while((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            struct stat filestat;
            char ppath[500];
            snprintf(ppath, 500, "%s/%s", path, entry->d_name);
            if (stat(ppath, &filestat) == -1) {
                continue;
            }
            if(greater_than_size==1){
                if(S_ISREG(filestat.st_mode) && filestat.st_size>given_size){
                    printf("\n%s", ppath);
                }
            }
            else if(has_perm_execute==1){
                if ((filestat.st_mode & S_IXUSR) == S_IXUSR) {
                    printf("\n%s", ppath);
                }
            }else
                printf("\n%s", ppath);
            if(S_ISDIR(filestat.st_mode)) {
                listDirRec(ppath,has_perm_execute,greater_than_size,given_size,iteratie+1);
            }                   
        }
    }
    closedir(dir);
}

void parse_file(char* path){
    int fd;
    fd=open(path,O_RDONLY);
    if(fd==-1){
        return;
    }
    char magic;
    lseek(fd,-1,SEEK_END);
    ssize_t magic_read=read(fd,&magic,1);
    if(magic_read == -1){
        return;
    }
    if(magic=='X'){
        lseek(fd,-3,SEEK_END);
        short header_size;
        ssize_t hsize_read=read(fd,&header_size,2);
        if(hsize_read == -1){
            close(fd);
            return;
        }
        lseek(fd,-header_size,SEEK_END);
        short version;
        ssize_t version_read=read(fd,&version,2);
        if(version_read == -1){
            close(fd);
            return;
        }
        if(version<24 || version >68){
            printf("ERROR\nwrong version");
            close(fd);
            return;
        }
        else{
            char no_of_sections;
            ssize_t no_sect_read=read(fd,&no_of_sections,1);
            if(no_sect_read == -1){
                close(fd);
                return;
            }
            if(no_of_sections<8 || no_of_sections>15){
                printf("ERROR\nwrong sect_nr");
                close(fd);
                return;
            }
            else{
                section_header* sections=(section_header*)calloc(no_of_sections,sizeof(section_header));
                for(int i=0;i<no_of_sections;i++){
                    ssize_t sect_read=read(fd,sections[i].sect_name,11);
                    if(sect_read==-1){
                        close(fd);
                        return;
                    }
                    sect_read=read(fd,&sections[i].sect_type,1);
                    if(sect_read==-1){
                        close(fd);
                        return;
                    }
                    if(sections[i].sect_type<64 || sections[i].sect_type>65){
                        printf("ERROR\nwrong sect_types");
                        close(fd);
                        free(sections);
                        return;
                    }
                    sect_read=read(fd,&sections[i].sect_offset,4);
                    if(sect_read==-1){
                        close(fd);
                        return;
                    }
                    sect_read=read(fd,&sections[i].sect_size,4);
                    if(sect_read==-1){
                        close(fd);
                        return;
                    }
                }
                printf("SUCCESS\n");
                printf("version=%d\n",version);
                int aux=(int)no_of_sections;
                printf("nr_sections=%d",aux);
                for(int i=0;i<aux;i++){
                    printf("\nsection%d: %s %d %d",i+1,sections[i].sect_name,sections[i].sect_type,sections[i].sect_size);
                }
                free(sections);
            }
        }
    }
    else{
        printf("ERROR\nwrong magic");
        close(fd);
        return;
    }
    if(close(fd)==-1){
        return;
    }
}

void decide(int parse, int list, int recursive, int greater_than_size, int has_perm_execute, char* path, int given_size){
    if(list == 1){
        if(recursive==1){
            if(greater_than_size==1){
                int iteratie=1;
                listDirRec(path,has_perm_execute,greater_than_size,given_size, iteratie);
            }else{
                int iteratie=1;
                listDirRec(path,has_perm_execute,greater_than_size,given_size, iteratie);
            }
        } else{
            if(greater_than_size==1){
                listDir(path,has_perm_execute,greater_than_size,given_size);
            }else{
                listDir(path,has_perm_execute,greater_than_size,given_size);
            }
        }
    } else if(parse==1){
        parse_file(path);
    }else
        return;
}

int main(int argc, char **argv){
    if(argc >= 2){
        if(strcmp(argv[1], "variant") == 0){
            printf("36931\n");
        }
        else if(strcmp(argv[1], "extract") == 0){

        }
        else if(strcmp(argv[1], "findall") == 0){
        
        }
        else{
            int list=0;
            int recursive=0;
            int greater_than_size=0;
            int has_perm_execute=0;

            int parse=0;

            char* dir_path=NULL;
            int given_size=0;

            for(int i=1;i<argc;i++){
                if(strcmp(argv[i], "list")==0){
                    list=1;
                }
                else if(strcmp(argv[i], "recursive")==0){
                    recursive=1;
                }
                else if(strcmp(argv[i], "has_perm_execute")==0){
                    has_perm_execute=1;
                }
                else if(strcmp(argv[i], "parse")==0){
                    parse=1;
                }
                else{
                    char* possible_path=argv[i];
                    char* path_prefix="path=";
                    size_t path_prefix_len=strlen(path_prefix);
                    if(strncmp(possible_path,path_prefix,path_prefix_len)==0){
                        dir_path=possible_path+path_prefix_len;
                    }
                    char* sz_prefix="size_greater=";
                    size_t sz_prefix_len=strlen(sz_prefix);
                    if(strncmp(possible_path,sz_prefix,sz_prefix_len)==0){
                        char* gr_than=possible_path+sz_prefix_len;
                        sscanf(gr_than,"%d",&given_size);
                        greater_than_size=1;
                    }
                }
            }
            decide(parse,list,recursive,greater_than_size,has_perm_execute,dir_path, given_size);
        }

    }
    return 0;
}
