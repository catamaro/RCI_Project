#include "connect.h"

int main(int argc, char *argv[]){
	struct addrinfo hints,*res;
	int fd,newfd,afd=0,errcode;
	ssize_t n,nw;
	struct sockaddr_in addr;
	socklen_t addrlen;
	char *ptr,buffer[128];
	fd_set rfds;
	enum {idle,busy} state;
	int maxfd,counter;

	if((fd=socket(AF_INET,SOCK_STREAM,0))==-1)exit(1);//error

	memset(&hints,0,sizeof hints);
	hints.ai_family=AF_INET;//IPv4
	hints.ai_socktype=SOCK_STREAM;//TCP socket
	hints.ai_flags=AI_PASSIVE;
	
	if((errcode= getaddrinfo (NULL,"58001",&hints,&res))!=0)/*error*/exit(1);

	if( bind (fd,res->ai_addr,res->ai_addrlen)==-1)/*error*/exit(1);
	if( listen (fd,5)==-1)/*error*/exit(1);

	state=idle;
	while(1){FD_ZERO(&rfds);
		FD_SET(fd,&rfds);maxfd=fd;
		if(state==busy){FD_SET(afd,&rfds);maxfd=max(maxfd,afd);}

		counter=select(maxfd+1,&rfds,(fd_set*)NULL,(fd_set*)NULL,(struct timeval *)NULL);
		if(counter<=0)/*error*/exit(1);
		
		if(FD_ISSET(fd,&rfds))
		{
			addrlen=sizeof(addr);
			if((newfd=accept(fd,(struct sockaddr*)&addr,&addrlen))==-1)/*error*/exit(1);
			switch(state)
			{
				case idle: afd=newfd;state=busy;break;
				case busy: /* ... *///write “busy\n” in newfd
					close(newfd); break;
			}
		}
		if(FD_ISSET(afd,&rfds))
		{
			if((n=read(afd,buffer,128))!=0)
			{
				if(n==-1)/*error*/exit(1);
				write(1,"received: ",10);write(1,buffer,n);
				n=write(afd,buffer,n);
				if(n==-1)/*error*/exit(1);
			}
			else{close(afd);state=idle;}//connection closed by peer
		}
	}//while(1)
	/*close(fd);exit(0);*/
}