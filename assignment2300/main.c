
#define _POSIX_C_SOURCE 200809L  

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <poll.h>
#include "list.h"
#include <netdb.h>
#include <unistd.h>			
#include <sys/types.h>
#include "peerthread.h"
#include "keyboardthread.h"
#include "serverthread.h"
#include "screenthread.h"


int main(int argc, char** argv){

    int sd;
    struct sockaddr_in server;
    struct sockaddr_in* ptr;
    unsigned long int x;
    struct sockaddr_in peer;
    unsigned long int y;
    int check;
    struct addrinfo hint, *final = NULL;

    //set all values in struct to 0
    hint.ai_family = AF_UNSPEC;
    hint.ai_flags = 0;
    hint.ai_socktype = SOCK_DGRAM;
    hint.ai_protocol = 0;
    memset(&hint,0,sizeof(struct addrinfo));

    //bind address and setup for peer
    int var = getaddrinfo(argv[2],argv[3],&hint,&final);
    ptr = (struct sockaddr_in*)final->ai_addr;
    peer.sin_family = AF_INET;
    x = strtoul(argv[3],NULL,0);

    if(x == 0){

        printf("invalid strtoul");
    }

    peer.sin_port = htons(x);
    char* charptr = inet_ntoa((struct in_addr)ptr->sin_addr); //used to convert address to IPV4

    if(charptr == NULL){

        printf("conversion of address to IPV4 failed\n");

    }

    argv[2] = charptr; //set the address
    check = inet_aton(argv[2],&peer.sin_addr);

    if(check == 0){
        printf("error invalid remote address %s\n",argv[2]);
        return 1;

    }

    //make socket descriptor
    sd = socket(AF_INET,SOCK_DGRAM,0);
    if(sd < 0){
        printf("error opening socket");
        return 1;
    }

    //set up for server
    server.sin_port = htons(strtoul(argv[1],NULL,0));
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_family = AF_INET;
    int binder = bind(sd,(struct sockaddr *)(&server),sizeof(server));

    //check if socket bind is successful
    if(binder < 0){
        printf("failed to bind socket");
        return 1;

    }

    //initialize all threads to begin input
    keyboardThreadSetup();
    sendthreadsetup(argv,ptr,sd);
    screenthreadsetup();
    recievethreadsetup(sd);
    keyboardThreadEnd();
    close(sd);
    freeaddrinfo(final);
    //free memory allocated
    free(final);
    return 0;
    
}
