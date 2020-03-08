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

#define max(A,B) ((A)>=(B)?(A):(B))

int TCP_CLIENT ();

#endif