#include "connect.h"

int main(void)
{
	int fd = TCP_CLIENT ();
	char *ptr,buffer[128];
	ptr=strcpy(buffer,"Hello!\n");
	ssize_t nbytes, nleft,nwritten,nread;
	nbytes=7;

	nleft=nbytes;
	while(nleft>0){
		nwritten=write(fd,ptr,nleft);
		if(nwritten<=0)/*error*/exit(1);
		nleft-=nwritten;
		ptr+=nwritten;}
	nleft=nbytes; ptr=buffer;
	while(nleft>0){
		nread=read(fd,ptr,nleft);
		if(nread==-1)/*error*/exit(1);
		else if(nread==0)break;//closed by peer
		nleft-=nread;
		ptr+=nread;}
	nread=nbytes-nleft;
	close(fd);

	write(1,"echo: ",6);//stdout
	write(1,buffer,nread);
	exit(0);
}

