#include "connect.h"
    
int TCP_CLIENT (){
    
    int fd;
    ssize_t n;

    struct addrinfo hints, *res;
    struct sigaction act;

    fd=socket(AF_INET,SOCK_STREAM,0);//TCP socket
	if(fd==-1){perror("tcp socket err:");exit(1);}

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;//IPv4
    hints.ai_socktype=SOCK_STREAM;//TCP socket

    memset(&act,0,sizeof act);
	act.sa_handler=SIG_IGN;

	if(sigaction(SIGPIPE,&act,NULL)==-1)/*error*/exit(1);

    n = getaddrinfo ("127.0.1.1","58001", &hints, &res); // localhost vai ser tejo ou enedereço do pai
    if(n!=0){perror("tcp getaddrinfo err:");exit(1);}
   
    n = connect (fd, res->ai_addr, res->ai_addrlen);
    if(n==-1){perror("fonte err:");exit(1);}
  	
	return fd;      
}

int TCP_SERVER (char* port){
    
    int fd;
    ssize_t n;

    struct addrinfo hints, *res;
    struct sigaction act;

    fd=socket(AF_INET,SOCK_STREAM,0);//TCP socket
	if(fd==-1){perror("tcp socket err:");exit(1);}

	memset(&hints,0,sizeof hints);
	hints.ai_family=AF_INET;//IPv4
	hints.ai_socktype=SOCK_STREAM;//TCP socket
	hints.ai_flags=AI_PASSIVE;

    memset(&act,0,sizeof act);
	act.sa_handler=SIG_IGN;

	if(sigaction(SIGPIPE,&act,NULL)==-1)/*error*/exit(1);

    n = getaddrinfo (NULL,port,&hints,&res); // localhost vai ser tejo ou enedereço do pai
    if(n!=0){perror("tcp getaddrinfo err:");exit(1);}

	n = bind (fd,res->ai_addr,res->ai_addrlen);
    if(n==-1){perror("bind err:");exit(1);}

	n = listen (fd,5);
    if(n==-1){perror("listen err:");exit(1);}
  	
	return fd;      
}

int UDP_SERVER (char* port){
    
    int fd;
    ssize_t n;

    struct addrinfo hints, *res;
    struct sigaction act;

    fd=socket(AF_INET,SOCK_DGRAM,0);//UDP socket
	if(fd==-1){perror("tcp socket err:");exit(1);}

	memset(&hints,0,sizeof hints);
	hints.ai_family=AF_INET;//IPv4
	hints.ai_socktype=SOCK_DGRAM;//UDP socket
	hints.ai_flags=AI_PASSIVE;

    memset(&act,0,sizeof act);
	act.sa_handler=SIG_IGN;

	if(sigaction(SIGPIPE,&act,NULL)==-1)/*error*/exit(1);

    n = getaddrinfo (NULL,port,&hints,&res); // localhost vai ser tejo ou enedereço do pai
    if(n!=0){perror("udp getaddrinfo err:");exit(1);}

	n = bind (fd,res->ai_addr,res->ai_addrlen);
    if(n==-1){perror("bind err:");exit(1);}
  	
	return fd;      
}

