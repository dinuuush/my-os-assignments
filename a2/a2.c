#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <string.h>
#include <pthread.h>

struct ThreadArgs {
    int processNr;
    int threadNr;
};

void* thread_function(void* arg){
    struct ThreadArgs* args = (struct ThreadArgs*)arg;
    int processNr = args->processNr;
    int threadNr = args->threadNr;
    free(arg);

    // possible thread’ actions

    // inform the tester about thread start
    info(BEGIN, processNr, threadNr);

    // other possible thread’ actions

    // inform the tester about thread termination
    info(END, processNr, threadNr);
    return NULL;
}

int main(){
    pthread_t* p7_threads = (pthread_t*)malloc(sizeof(pthread_t)*5);
    //tester initialization
    //only one time in the main process
    init();
    // create process P1
    
    //int pidP1 = getpid();

    //inform the tester about main process' start
    info(BEGIN, 1, 0);
    int pidP2 = fork();

    if(pidP2 == 0){ // P2 process
        info(BEGIN, 2, 0);
        int pidP5 = fork();
        if(pidP5 == 0){ // P5 process
            info(BEGIN, 5, 0);
            info(END, 5, 0);
            exit(0);
        }
        int pidP8 = fork();
        if(pidP8 == 0){ // P8 process
            info(BEGIN, 8, 0);
            info(END, 8, 0);
            exit(0);
        }
        waitpid(pidP5, NULL, 0);
        waitpid(pidP8, NULL, 0);
        info(END, 2, 0);
        exit(0);
    }

    int pidP3 = fork();
    if(pidP3 == 0){ // P3 process
        info(BEGIN, 3, 0);
        info(END, 3, 0);
        exit(0);
    }

    int pidP4 = fork();
    if(pidP4 == 0){ // P4 process
        info(BEGIN, 4, 0);
        info(END, 4, 0);
        exit(0);
    }

    int pidP6 = fork();
    if(pidP6 == 0){ // P6 process
        info(BEGIN, 6, 0);
        int pidP7 = fork();
        if(pidP7 == 0){ // P7 process
            info(BEGIN, 7, 0);
            for(int i=1; i<=4; i++){
                struct ThreadArgs* info = (struct ThreadArgs*)malloc(sizeof(struct ThreadArgs));
                info->processNr = 7;
                info->threadNr = i;
                pthread_create(&p7_threads[i], NULL, thread_function, (void*)info);
            }
            for(int i=1; i<=4; i++){
                pthread_join(p7_threads[i], NULL);
            }
            info(END, 7, 0);
            exit(0);
        }
        waitpid(pidP7, NULL, 0);
        info(END, 6, 0);
        exit(0);
    }

    waitpid(pidP2, NULL, 0);
    waitpid(pidP3, NULL, 0);
    waitpid(pidP4, NULL, 0);
    waitpid(pidP6, NULL, 0);
    //inform the tester about main process' termination
    info(END, 1, 0);
    return 0;
}
