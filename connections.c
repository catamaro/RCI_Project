#include "main.h"
    
int TCP_CLIENT (char* IP, char* port){
    
    int fd;
    ssize_t n;

    struct addrinfo hints, *res;
    struct sigaction act;

    fd=socket(AF_INET,SOCK_STREAM,0);//TCP socket
	if(fd==-1){
        perror("error: on tcp socket");
        exit(1);
    }

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;//IPv4
    hints.ai_socktype=SOCK_STREAM;//TCP socket

    memset(&act,0,sizeof act);
	act.sa_handler=SIG_IGN;

	if(sigaction(SIGPIPE,&act,NULL) == -1) exit(0);

    n = getaddrinfo (IP, port, &hints, &res); 
    if(n != 0 ){
        perror("error: on tcp getaddrinfo");
        exit(1);
    }
   
    n = connect (fd, res->ai_addr, res->ai_addrlen);
    if(n == -1){
        perror("error: on tcp connect");
        exit(1);
    }
  	freeaddrinfo(res);

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

	if(sigaction(SIGPIPE,&act,NULL) == -1) exit(0);

    n = getaddrinfo (NULL,port,&hints,&res); 
    if(n != 0){
        perror("error: on tcp getaddrinfo");
        exit(1);
    }

	n = bind (fd,res->ai_addr,res->ai_addrlen);
    //when bind fails program returns -1 because the tcp connection
    //is established when there is a entry, sentry or new so if it 
    //fails it can be added again further on the program
    if(n == -1){
        perror("error: on tcp bind");
        printf("please try and wait until tcp address is complety reset\n");
	    freeaddrinfo(res);
        return -1;
    }

	n = listen (fd,5);
    if(n == -1){
        perror("error: on tcp listen");
        exit(1);
    }
    freeaddrinfo(res);
  	
	return fd;      
}

struct addrinfo* UDP_CLIENT(char* IP, char* port, int fd){
    int errcode;
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      //IPv4
    hints.ai_socktype = SOCK_DGRAM; //UDP socket
    hints.ai_flags=AI_PASSIVE;

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

    fd = socket(AF_INET,SOCK_DGRAM,0);//UDP socket
	if(fd == -1){
        perror("error: on udp socket");
        exit(1);
    }

	memset(&hints,0,sizeof hints);
	hints.ai_family=AF_INET;//IPv4
	hints.ai_socktype=SOCK_DGRAM;//UDP socket
	hints.ai_flags=AI_PASSIVE;

    n = getaddrinfo (NULL,port,&hints,&res);
    if(n != 0){
        perror("error: on udp getaddrinfo");
        exit(1);
    }

	n = bind (fd,res->ai_addr,res->ai_addrlen);
    //when bind fails program exits because the udp connection
    //could never more be established once it is on the beggining 
    //of the code 
    if(n == -1){
        perror("error: on udp bind");
        printf("please try and wait until udp address is complety reset\n");
        exit(1);
    }

  	freeaddrinfo(res);

	return fd;      
}
