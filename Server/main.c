#include "connect.h"

int main(int argc, char *argv[]){

	int fd_server_tcp, fd_server_udp,succ_fd = -1, incoming_fd = -1, pred_fd = -1;
	ssize_t n;
	struct sockaddr_in addr;
	socklen_t addrlen;
	char buffer[128];
	fd_set read_fds;
	int maxfd,retval;
	int flag_pred_out = 0;
	char IP[128], port[128];

	
	//exit when wrong #arguments
	if(argc != 3){
		printf("error: command of type dkt <IP> <port>\n");
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
	
	//init node_key to default
	server_state.node_key = -1;
	server_state.succ_fd = -1;
	server_state.pred_fd = -1;

	while(1){
		memset(buffer, 0, strlen(buffer));
		//reset set of read file descriptors
        FD_ZERO(&read_fds);

		//add keyboard fd to set
        FD_SET(STDIN_FILENO,&read_fds);
        maxfd = STDIN_FILENO;

		//add TCP fd to set 
		//este fd basicamente está à espera de novas coneções por TCP
		FD_SET(fd_server_tcp,&read_fds);
        maxfd = max(maxfd,fd_server_tcp);

		//add UDP fd to set
		FD_SET(fd_server_udp,&read_fds);
        maxfd = max(maxfd,fd_server_udp);

		//uma nova ligação aceite vai dar origem a um novo fd para a comunicação
		if(incoming_fd != -1){
			FD_SET(incoming_fd,&read_fds);
        	maxfd = max(maxfd,incoming_fd);
		}

		//add TCP connecion to set when there is a connection with the sucessor
		succ_fd = server_state.succ_fd;
		if(succ_fd != -1){
			FD_SET(succ_fd,&read_fds);
        	maxfd = max(maxfd,succ_fd);
		}

		//add TCP connecion to set when there is a connection with the predecessor
		pred_fd = server_state.pred_fd;
		if(pred_fd != -1){
			FD_SET(pred_fd,&read_fds);
        	maxfd = max(maxfd,pred_fd);
		}

		//blocks until one fd is ready to read **or until timer ends**
		retval = select(maxfd+1,&read_fds,(fd_set*)NULL,(fd_set*)NULL,(struct timeval *)NULL);
		if(retval <= 0){
			printf("error: empty select");
			exit(1);
		}
		
        //read from keyboard
		if(FD_ISSET(STDIN_FILENO,&read_fds)){
			if(fgets(buffer, 128, stdin) == NULL){
				printf("error: reading from keyboard\n");
				exit(1);
			}
			
			//se retornar -2 o new funcionou mas como és o único serve no anel não existe succ e não vai dar set
			retval = interface_utilizador(buffer, IP, port);
			if(retval == -2) printf("Ring created successfully\n");
			else if(retval == -3) printf("Server entered successfully on the ring\n");
			else if(retval == -4) printf("Show completed\n");
			else if(retval == -7){
				close(fd_server_udp);
				close(fd_server_tcp);
				close(incoming_fd);
				return 0;
			}
				
		}
		//receive message from new server
		if(incoming_fd != -1){
			if(FD_ISSET(incoming_fd,&read_fds))
			{
				if((n = read(incoming_fd,buffer,128)) != 0)
				{
					if(n==-1){
						printf("error: cannot read from keyboard");
						exit(1);
					}
					message_incoming_fd(buffer, incoming_fd, &flag_pred_out);
					incoming_fd = -1;
				}
				else{
					close(incoming_fd);//connection closed by peer
					incoming_fd = -1;
				}
			}
		}
		//receive message from sucessor
		if(succ_fd != -1){
			if(FD_ISSET(succ_fd,&read_fds)){
				if((n = read(succ_fd,buffer,128)) != 0){
					if(n==-1){
						printf("error: cannot read message from sucessor");
						exit(1);
					}
					message_succ_fd(buffer);
				}
				else{
					close(succ_fd);//connection closed by peer
					server_state.succ_fd = -1;
					reconnection_succ();
				}
			}
		}
		//receive tcp message from predecessor
		if(pred_fd != -1){
			if(FD_ISSET(pred_fd,&read_fds))
			{
				if((n=read(pred_fd,buffer,128))!=0)
				{
					if(n==-1){
						printf("error: cannot read message from predecessor");
						exit(1);
					}
					message_pred_fd(buffer);
				}
				else{
					close(pred_fd);//connection closed by peer
					server_state.pred_fd = -1;
					flag_pred_out = 1;
				}
			}
		}
		//create tcp connection
		if(FD_ISSET(fd_server_tcp,&read_fds))
		{	
			addrlen = sizeof(addr);
			incoming_fd = accept(fd_server_tcp,(struct sockaddr*)&addr,&addrlen);
			if(incoming_fd == -1){
				printf("error: cannot read message from incoming fd");
				exit(1);
			}
		}
		//receive udp message
		/*if(FD_ISSET(fd_server_udp,&read_fds))
		{
			n = recvfrom(fd_server_udp,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
			if(n==-1)exit(1);

			printf("udp_received: %s", buffer);
			retval = interface_utilizador(buffer, NULL, NULL);
			//n = sendto(fd_server_udp,"Key is being searched\n",n,0,(struct sockaddr*)&addr,addrlen);

		}*/
	}
}
