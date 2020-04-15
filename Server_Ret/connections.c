#include "main.h"
    
int TCP_CLIENT (char* IP, char* port){
    
    int fd;
    ssize_t n;

    struct addrinfo hints, *res;
    struct sigaction act;

    fd=socket(AF_INET,SOCK_STREAM,0);//TCP socket
	if(fd==-1){
        perror("error: on tcp socket");
        return -1;
    }

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;//IPv4
    hints.ai_socktype=SOCK_STREAM;//TCP socket

    memset(&act,0,sizeof act);
	act.sa_handler=SIG_IGN;

	if(sigaction(SIGPIPE,&act,NULL) == -1) return -1;

    n = getaddrinfo (IP, port, &hints, &res); 
    if(n!=0){
        perror("error: on tcp getaddrinfo");
        return -1;
    }
   
    n = connect (fd, res->ai_addr, res->ai_addrlen);
    if(n==-1){
        perror("error: on tcp connect");
        
        exit(0);
    }
  	
	return fd;      
}

int TCP_SERVER (char* port){
    
    int fd;
    ssize_t n;

    struct addrinfo hints, *res;
    struct sigaction act;

    fd = socket(AF_INET,SOCK_STREAM,0);//TCP socket
	if(fd == -1){
        perror("error: on tcp socket");
        return -1;
    }

	memset(&hints,0,sizeof hints);
	hints.ai_family = AF_INET;//IPv4
	hints.ai_socktype = SOCK_STREAM;//TCP socket
	hints.ai_flags = AI_PASSIVE;

    memset(&act,0,sizeof act);
	act.sa_handler=SIG_IGN;

	if(sigaction(SIGPIPE,&act,NULL) == -1) return -1;

    n = getaddrinfo (NULL,port,&hints,&res); // localhost vai ser tejo ou enedereço do pai
    if(n != 0){
        perror("error: on tcp getaddrinfo");
        return -1;
    }

	n = bind (fd,res->ai_addr,res->ai_addrlen);
    if(n==-1){
        perror("error: on tcp bind");
        return -1;
    }

	n = listen (fd,5);
    if(n==-1){
        perror("error: on tcp listen");
        return -1;
    }
  	
	return fd;      
}

struct addrinfo* UDP_CLIENT(char* IP, char* port, int *fd){
    int errcode;
    struct addrinfo hints, *res;

    *fd = socket(AF_INET, SOCK_DGRAM, 0); //UDP socket

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      //IPv4
    hints.ai_socktype = SOCK_DGRAM; //UDP socket

    errcode = getaddrinfo(IP, port, &hints, &res);
    if(errcode != 0){
        perror("error: on udp getaddrinfo");
        exit(1);
    }

    return res;
}

int UDP_SERVER (char* port){
    
    int fd;
    ssize_t n;

    struct addrinfo hints, *res;
    struct sigaction act;

    fd=socket(AF_INET,SOCK_DGRAM,0);//UDP socket
	if(fd==-1){
        perror("error: on udp socket");
        return -1;
    }

	memset(&hints,0,sizeof hints);
	hints.ai_family=AF_INET;//IPv4
	hints.ai_socktype=SOCK_DGRAM;//UDP socket
	hints.ai_flags=AI_PASSIVE;

    memset(&act,0,sizeof act);
	act.sa_handler=SIG_IGN;

	if(sigaction(SIGPIPE,&act,NULL)==-1) return -1;;

    n = getaddrinfo (NULL,port,&hints,&res); // localhost vai ser tejo ou enedereço do pai
    if(n != 0){
        perror("error: on udp getaddrinfo");
        return -1;
    }

	n = bind (fd,res->ai_addr,res->ai_addrlen);
    if(n==-1){
        perror("error: on udp bind");
        return -1;
    }
  	
	return fd;      
}

