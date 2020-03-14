#include "connect.h"

int main(int argc, char *argv[]){
	struct addrinfo hints,*res;
	int fd_server_tcp, fd_server_udp,comunication_tcp_fd = -1,errcode;
	ssize_t n,nw;
	struct sockaddr_in addr;
	socklen_t addrlen;
	char *ptr,buffer[128], fgets_buffer[1024];
	fd_set rfds;
	int maxfd,counter;
    int size;

	char IP[128], port[128];
	
	if(argc != 3){
		write(1,"dkt <IP> <port>\n", 16);
		exit(0);
	}
	else{
		strcpy(IP, argv[1]);
		strcpy(port, argv[2]);
	}

    fd_server_tcp = TCP_SERVER(port);
    fd_server_udp = UDP_SERVER(port);


	while(1){
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO,&rfds);
        maxfd=STDIN_FILENO;

		FD_SET(fd_server_tcp,&rfds);
        maxfd=max(maxfd,fd_server_tcp);

		FD_SET(fd_server_udp,&rfds);
        maxfd=max(maxfd,fd_server_udp);

		if(comunication_tcp_fd != -1){
			FD_SET(comunication_tcp_fd,&rfds);
        	maxfd=max(maxfd,comunication_tcp_fd);
		}

		counter=select(maxfd+1,&rfds,(fd_set*)NULL,(fd_set*)NULL,(struct timeval *)NULL);
		if(counter<=0)/*error*/exit(1);
		
        // Lê do teclado
		if(FD_ISSET(STDIN_FILENO,&rfds)){
			if(fgets(fgets_buffer, 1024, stdin) == NULL){
				printf("error reading from keyboard");
				exit(0);
			}
			
   			// Lê os comandos do teclado e ativa a opção necessária
			interface_utilizador(fgets_buffer, IP, port);
		}

		// cria servidor tcp
		else if(FD_ISSET(fd_server_tcp,&rfds))
		{
			addrlen=sizeof(addr);
			if((comunication_tcp_fd=accept(fd_server_tcp,(struct sockaddr*)&addr,&addrlen))==-1)/*error*/exit(1);
		}

		// recebe uma mensagem por tcp
		else if(FD_ISSET(comunication_tcp_fd,&rfds))
		{
			if((n=read(comunication_tcp_fd,buffer,128))!=0)
			{
				if(n==-1)/*error*/exit(1);
				write(1,"tcp_received: ",14);write(1,buffer,n);
				n=write(comunication_tcp_fd,buffer,n);
				if(n==-1)/*error*/exit(1);
			}
			else{close(comunication_tcp_fd);}//connection closed by peer
		}

		// recebe uma mensagem por udp
		else if(FD_ISSET(fd_server_udp,&rfds))
		{
			n=recvfrom(fd_server_udp,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
			if(n==-1)/*error*/exit(1);
			write(1,"udp_received: ",14);write(1,buffer,n);
			n=sendto(fd_server_udp,buffer,n,0,(struct sockaddr*)&addr,addrlen);
			if(n==-1)/*error*/exit(1);
		}
		
		else{

		}
	}//while(1)
	/*close(fd);exit(0);*/
}