#ifndef MAIN_H
#define MAIN_H

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
#include <time.h>

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
    int succ_fd;
    int pred_fd;
} relations;

relations server_state;
relations auxiliar;

//functions of connection.c
int TCP_CLIENT (char* IP, char* port);
int TCP_SERVER (char* port);
struct addrinfo* UDP_CLIENT(char* IP, char* port, int *fd);
int UDP_SERVER (char* port);

//functions of operations.c
int interface_utilizador(char* comando_utilizador, char* IP, char* port);
int new(char* comando_utilizador, char* IP, char* port);
int sentry(char* comando_utilizador, char* IP, char* port);
int find(char* comando_utilizador);
int leave();
int entry(char* comando_utilizador, char* IP, char* port);
void send_message(int fd, int node_key, char* IP, char* port, char* comand);
void send_find_message(int fd, int node_key, char* IP, char* port, char* comand, int search_key);
void send_message_udp(int fd, int node_key, struct addrinfo *res, char* IP, char* port, char* comand, int search_key);
int tcp_read(int fd, char* buffer);

//functions of functions.c
int message_incoming_fd(char* message, int incoming_fd, int* flag_pred_out, struct sockaddr_in udp_addr, int fd_server_udp, int* flag_udp);
int message_succ_fd(char* message);
int message_pred_fd(char* message);
struct sockaddr_in message_udp(int fd_server_udp);
int succ_NEW(char* message);
int succ_SUCC(char* message);
int succ_FND(char* message);

int reconnection_succ();

#endif