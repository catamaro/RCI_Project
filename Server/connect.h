#ifndef CONNECT_H
#define CONNECT_H

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

#define N 32

//struct with information of server state
typedef struct relations
{
    int node_key; //key of the server
    char node_IP[16];
    char node_TCP[8];
    int succ_key; //key of the server 1st sucessor
    char succ_IP[16];
    char succ_TCP[8];
    int succ2_key; //key of the server 2nd sucessor
    char succ2_IP[16];
    char succ2_TCP[8];
} relations;

relations server_state;

int TCP_CLIENT (char* IP, char* port);

int TCP_SERVER (char* port);

int UDP_SERVER (char* port);

int interface_utilizador(char* comando_utilizador, char* IP, char* port);

void new(char* comando_utilizador, char* IP, char* port);

int sentry(char* comando_utilizador, char* IP, char* port);

#endif