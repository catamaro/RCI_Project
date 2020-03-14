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

typedef struct
{
    int node_key;
    char node_IP[16];
    char node_TCP[8];
    int succ_key;
    char succ_IP[16];
    char succ_TCP[8];
    int succ2_key;
    char succ2_IP[16];
    char succ2_TCP[8];
} relations;

relations server_state;

int TCP_CLIENT ();

int TCP_SERVER (char* port);

int UDP_SERVER (char* port);

void interface_utilizador(char* comando_utilizador, char* IP, char* port);

#endif