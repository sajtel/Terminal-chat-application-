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
#include <unistd.h>			
#include <errno.h>
#include "list.h"
#include "keyboardthread.h"


#define max 1024
extern pthread_cond_t condvar;
extern pthread_mutex_t mutex;
static int socketDescriptor;
static pthread_t threadID;
extern LIST* keyboardlist;
static struct sockaddr_in peer;

void* sendthreadcreate(){

    char* message; //pointer to the message 
    int size; //stores number of bytes sent
    char store[max]; //buffer for message
    struct sockaddr_in* peerptr = &peer;//pointer to peer struct

    while(1){

        //we want to modify the shared list so we lock the mutex since we are using it
        pthread_mutex_lock(&mutex);
        {
            pthread_cond_wait(&condvar,&mutex);

            int listcounter = ListCount(keyboardlist);//check if list is empty

            //if list is not empty, retrieve item from list to send to server
            if(listcounter > 0){

                message = (char *)ListTrim(keyboardlist); //retrieve message from shared list and store in char* ptr
                strcpy(store,message); //copy message from char ptr to buffer
                printf("sending %s\n",store);
                
                //send message to server
                size = sendto(socketDescriptor,(const char*) store,strlen(store),0,(struct sockaddr *)peerptr,sizeof(struct sockaddr_in));
                memset(store,0,strlen(store)); //reset buffer for next message
                

                //checking if message sent
                if(size < 0){

                    printf("error sending message");

                }

            }
        }
        //done using the mutex so we can unlock it now
        pthread_mutex_unlock(&mutex);

    }

    return NULL;

}

void sendshutdown(){

    pthread_cancel(threadID);
    pthread_join(threadID,NULL);
    

}

void sendthreadsetup(char** argv,struct sockaddr_in *var,int sd){

    unsigned long int portresult = 0;
    int ipresult = 0;
    socketDescriptor = sd;
    peer.sin_family = AF_INET;
    portresult = strtoul(argv[3],NULL,0);

    //checking successful stroul
    if(portresult == 0){
        printf("error, cannot convert number");
    }
    //assigning the port
    peer.sin_port = htons(portresult);
    ipresult = inet_aton(argv[2],&peer.sin_addr);

    //checking validity of address
    if(ipresult == 0){
        printf("error invalid address %s\n",argv[2]);
    }


    pthread_create(&threadID,NULL,sendthreadcreate,NULL);





}