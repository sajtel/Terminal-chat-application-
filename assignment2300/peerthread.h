#ifndef _PEERTHREAD_H_
#define _PEERTHREAD_H_

void sendthreadsetup(char **argv, struct sockaddr_in *var, int socketdescriptor);

void sendshutdown();

#endif