#include "connect.h"

int main(int argc, char *argv[]){

	struct addrinfo hints,*res;
	int fd_server_tcp, fd_server_udp,comunication_tcp_fd = -1,errcode;
	ssize_t n,nw;
	struct sockaddr_in addr;
	socklen_t addrlen;
	char *ptr,buffer[128], fgets_buffer[1024];
	fd_set read_fds;
	int maxfd,retval;
    int size;

	char IP[128], port[128];
	
	//exit when wrong #arguments
	if(argc != 3){
		write(1,"error: command of type dkt <IP> <port>\n", 16);
		exit(1);
	}
	//save IP and Port if right #arguments
	else{
		strcpy(IP, argv[1]);
		strcpy(port, argv[2]);
	}

	//inicialize TCP and UDP comunication for 1st server
    fd_server_tcp = TCP_SERVER(port);
    fd_server_udp = UDP_SERVER(port);

	while(1){
		//reset set of read file descriptors
        FD_ZERO(&read_fds);

		//add keyboard fd to set
        FD_SET(STDIN_FILENO,&read_fds);
        maxfd=STDIN_FILENO;

		//add TCP fd to set
		FD_SET(fd_server_tcp,&read_fds);
        maxfd=max(maxfd,fd_server_tcp);

		//add UDP fd to set
		FD_SET(fd_server_udp,&read_fds);
        maxfd=max(maxfd,fd_server_udp);

		//add client TCP fd to set if there is communication
		if(comunication_tcp_fd != -1){
			FD_SET(comunication_tcp_fd,&read_fds);
        	maxfd=max(maxfd,comunication_tcp_fd);
		}

		//blocks until one fd is ready to read **or until timer ends**
		retval=select(maxfd+1,&read_fds,(fd_set*)NULL,(fd_set*)NULL,(struct timeval *)NULL);
		if(retval<=0)/*error*/exit(1);

        //read from keyboard if ready
		if(FD_ISSET(STDIN_FILENO,&read_fds)){
			if(fgets(fgets_buffer, 1024, stdin) == NULL){
				printf("error reading from keyboard\n");
				exit(1);
			}
			
   			//activate action(e.g., new i)
			interface_utilizador(fgets_buffer, IP, port);
		}

		//create tcp connection if ready
		else if(FD_ISSET(fd_server_tcp,&read_fds))
		{
			addrlen=sizeof(addr);
			comunication_tcp_fd=accept(fd_server_tcp,(struct sockaddr*)&addr,&addrlen);
			if(comunication_tcp_fd==-1)/*error*/exit(1);
		}

		//receive tcp message
		else if(FD_ISSET(comunication_tcp_fd,&read_fds))
		{
			if((n=read(comunication_tcp_fd,buffer,128))!=0)
			{
				/*to test tcp connetcion
				if(n==-1)exit(1);
				write(1,"tcp_received: ",14);
				write(1,buffer,n);
				
				n=write(comunication_tcp_fd,buffer,n);
				if(n==-1)exit(1);*/
			}
			else{close(comunication_tcp_fd);}//connection closed by peer
		}

		//receive udp message
		else if(FD_ISSET(fd_server_udp,&read_fds))
		{
			n=recvfrom(fd_server_udp,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
			if(n==-1)/*error*/exit(1);
			write(1,"udp_received: ",14);write(1,buffer,n);

			n=sendto(fd_server_udp,buffer,n,0,(struct sockaddr*)&addr,addrlen);
			if(n==-1)/*error*/exit(1);
		}
		
		else{

		}
	}
	/*close(fd);
	exit(0);*/
}