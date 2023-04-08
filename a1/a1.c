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
            }
            else if(S_ISDIR(filestat.st_mode)) {
                listDirRec(ppath,has_perm_execute,greater_than_size,given_size,iteratie+1);
            } else{
                printf("\n%s", ppath);
            }    

        }
    }
    closedir(dir);
}
