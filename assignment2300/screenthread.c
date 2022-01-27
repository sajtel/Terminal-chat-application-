#define _POSIX_C_SOURCE 200809L  
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>			
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "list.h"
#include "keyboardthread.h"

extern LIST* printlist;
extern pthread_mutex_t recvmutex;
static pthread_t threadID;
extern pthread_cond_t recvcondvar;
int list;


void* screenthreadcreate(){

    char* mess;
    while(1){

        pthread_mutex_lock(&recvmutex); //lock mutex to modify list
        pthread_cond_wait(&recvcondvar,&recvmutex); //wait until item is added to list
        list = ListCount(printlist); //check to see if list is empty

        //if list is not empty, retrieve message
        if(list > 0){

            mess = (char*)ListRemove(printlist);
            puts(mess);

        }
        pthread_mutex_unlock(&recvmutex);

    }

    return NULL;

}

void screenshutdown(){

    pthread_cancel(threadID);
    pthread_join(threadID,NULL);


}

void screenthreadsetup(){

    pthread_create(&threadID,NULL,screenthreadcreate,NULL);

}