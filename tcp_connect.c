#include "connect.h"
    
int TCP_CLIENT (){
    
    int fd;
    ssize_t n;

    struct addrinfo hints, *res;
    struct sigaction act;

    fd=socket(AF_INET,SOCK_STREAM,0);//TCP socket
	if(fd==-1){perror("tcp socket err:");exit(1);}

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;
    hints.ai_flags=AI_NUMERICSERV;

    memset(&act,0,sizeof act);
	act.sa_handler=SIG_IGN;

	if(sigaction(SIGPIPE,&act,NULL)==-1)/*error*/exit(1);

    n = getaddrinfo ("127.0.1.1","58001", &hints, &res); // localhost vai ser tejo ou enedereço do pai
    if(n!=0){perror("tcp getaddrinfo err:");exit(1);}
   
    n = connect (fd, res->ai_addr, res->ai_addrlen);
    if(n==-1){perror("fonte err:");exit(1);}
  	
	return fd;      
}
