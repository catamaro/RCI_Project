#ifndef CLIENT_H
#define CLIENT_H

#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>

#define max(A,B) ((A)>=(B)?(A):(B))

int UDP_CLIENT (char* IP, char* port);

#endif