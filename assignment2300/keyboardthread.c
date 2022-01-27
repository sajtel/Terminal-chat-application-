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
#include "peerthread.h"
#include "serverthread.h"
#include "screenthread.h"

#define length 1024
LIST* keyboardlist;
static pthread_t threadID;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condvar = PTHREAD_COND_INITIALIZER;
pthread_t key;

void keyboardThreadEnd(){


    pthread_join(threadID,NULL);
    sendshutdown(); //this function is defined in sendthread.c, edit it appropriately
    recievethreadshutdown(); //same with this
    pthread_cancel(threadID);
    pthread_join(threadID,NULL);


}

void* keyboardthreadcreate()
{
    char message[length]; //hold the message 
    key = threadID;

    while(fgets(message,length,stdin)){

        pthread_mutex_lock(&mutex);
        ListPrepend(keyboardlist,&message); //adds item to the shared linked list
        pthread_cond_signal(&condvar);
        pthread_mutex_unlock(&mutex);

        //if user wants to end converation
        if(strncmp(message,"!",1) == 0){

            
            printf("Conversation ended!");
            keyboardThreadEnd();
            break;
        }

    }

    return NULL;
}

void keyboardThreadSetup(struct sockaddr_in *client, int descriptor){

    keyboardlist = ListCreate();
    pthread_create(&threadID,NULL,keyboardthreadcreate,NULL);

}



