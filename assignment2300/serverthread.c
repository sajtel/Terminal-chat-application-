#define _POSIX_C_SOURCE 200809L  
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>			// for close()
#include <errno.h>
#include "list.h"
#include "keyboardthread.h"

static int recvsocketdescriptor;
LIST* printlist;  //list to store recieved messages 
static pthread_t threadID; 
pthread_mutex_t recvmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t recvcondvar = PTHREAD_COND_INITIALIZER;
extern pthread_t key; //used to cancel the thread
int bytes; //number of bytes recieved
int list; //return value from adding to list


void* recvthreadcreate(){

    char message[1024];
    
    while(1){

        bytes = recvfrom(recvsocketdescriptor,message,sizeof(message),0,NULL,NULL);
        
        if(bytes < 0){

            //remember to change this 
            printf("error- recvfrom error: %s\n",strerror(errno));
            break;

        }

        pthread_mutex_lock(&recvmutex);

        list = ListInsert(printlist,message);

        if(strncmp(message,"!",1) == 0){

            printf("recieved a !. End of conversation");
            pthread_cancel(key);
            keyboardThreadEnd();
        }

        pthread_cond_signal(&recvcondvar);

        if(list == -1){
            printf("failed to insert item in printlist");
        }

        pthread_mutex_unlock(&recvmutex);

    }

    return NULL;

}

void recievethreadshutdown(){

    pthread_cancel(threadID);
    pthread_join(threadID,NULL);


}

void recievethreadsetup(int sd){

    printlist = ListCreate();
    recvsocketdescriptor = sd;
    pthread_create(&threadID,NULL,recvthreadcreate,NULL);

}