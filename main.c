#include "main.h"

int main(int argc, char *argv[]){

	char *IP, *port;
	bool running = true;

	//variables to store file descriptors
	int fd_server_tcp = -1, fd_server_udp,succ_fd = -1, incoming_fd = -1, pred_fd = -1;
	fd_set read_fds;
	int maxfd;

	//variables to store sockets
	struct sockaddr_in addr = { 0 };
	struct sockaddr_in udp_addr = { 0 };
	socklen_t addrlen;
	ssize_t n;

	//aux variables
	char *buffer = (char*) calloc(sizeof(char),128);
	char *last_message = (char*) calloc(sizeof(char),128);
	int flag_pred_out = 0, waiting_udp = 0, retval;
	
	//variables to control udp timer
	struct timeval *time_udp = NULL;
	int time_flag = 0;
	
	//exit when wrong #arguments
	if(argc != 3){
		printf("error: command of type dkt <IP> <port>\n");
		exit(1);
	}
	//save IP and Port if right #arguments
	else{
		IP = (char*) malloc(strlen(argv[1])*sizeof(char)+1);
		port = (char*) malloc(strlen(argv[2])*sizeof(char)+1);
		strcpy(IP, argv[1]);
		strcpy(port, argv[2]);
	}
	printf("The maximum number of keys is set at %d\n\n", N);

	//inicialize TCP and UDP comunication for 1st server
    fd_server_udp = UDP_SERVER(port);

	//init node_key to default
	server_state.node_key = -1;
	server_state.succ_fd = -1;
	server_state.pred_fd = -1;
	lost_message.fd = -1;
	lost_message.resent = 0;
	lost_message.ready = 0;

	//when running = false program exits orderly 
	while(running){
		//reset set of read file descriptors
        FD_ZERO(&read_fds);
		//add keyboard fd to set
        FD_SET(STDIN_FILENO,&read_fds);
        maxfd = STDIN_FILENO;

		//add TCP fd to set, waits for new TCP connections
		if(fd_server_tcp != -1){
			FD_SET(fd_server_tcp,&read_fds);
	    	maxfd = max(maxfd,fd_server_tcp);
		}

		//add UDP fd to set
		FD_SET(fd_server_udp,&read_fds);
        maxfd = max(maxfd,fd_server_udp);

		//an accepted connection creates a new fd for communication
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

		//blocks until one fd is ready to read or until udp timer is activated and it ends
		retval = select(maxfd+1,&read_fds,(fd_set*)NULL,(fd_set*)NULL, time_udp); //para por o timer a funcionar basta subsitituir o último NULL por &tv
		if(retval == -1){
			printf("error: error in select function\n");
			running = false;
		}
		else if(retval){

        	//read from keyboard
			if(FD_ISSET(STDIN_FILENO,&read_fds)){
				if(fgets(buffer, 128, stdin) == NULL){
					printf("error: reading from keyboard\n");
					running = false;
				}
				strcpy(last_message, buffer);

				//handler of possible returns regarding the input of the keyboard message
				n = interface_utilizador(buffer, IP, port, &fd_server_udp, &fd_server_tcp);
				//new is performed correctly
				if(n == -2) printf("Ring created successfully\n");
				//sentry is performed correctly
				else if(n == -3) printf("Server entered successfully on the ring\n");
				//show is performed correcly
				else if(n == -4) printf("Show completed\n");
				//entry is performed correcly
				else if(n == -6){
					time_udp = malloc(sizeof(struct timeval));
					(*time_udp).tv_sec = 5;
					(*time_udp).tv_usec = 0;
					time_flag = 0;
				}
				//leave is performed correctly
				else if(n == -7){
					close(fd_server_tcp);
					fd_server_tcp = -1;
					printf("Server leaved ring successfully\n");
					continue;
				} 
				//exit is performed correctly
				else if(n == -8){
					printf("Server leaved app successfully\n");
					running = false;
				}

				//reset the buffer for further reading
				memset(buffer, 0, strlen(buffer));
			}
			//receive message from new server
			if(incoming_fd != -1){
				if(FD_ISSET(incoming_fd,&read_fds))
				{
					if((n = tcp_read(incoming_fd, buffer)) == -1) 
						running = false;
					else if (n != 0){	
						//handles messages from server that are not direcly connected with server
						n = message_incoming(buffer, incoming_fd, &flag_pred_out, udp_addr, fd_server_udp, &waiting_udp);
						if (n == -1) running = false;
					
						incoming_fd = -1;

						//reset the buffer for further reading
						memset(buffer, 0, strlen(buffer));
					}
					//when returns 0 connection was closed by peer and closes as well
					else{
						close(incoming_fd);
						incoming_fd = -1;
					}
				}
			}
			//receive message from sucessor
			if(succ_fd != -1){
				if(FD_ISSET(succ_fd,&read_fds))
				{
					if((n = tcp_read(succ_fd, buffer)) == -1)
						running = false;
					else if (n != 0){
						//handles messages sent from sucessor
						n = message_succ(buffer);
						if (n == -1) running = false;

						//reset the buffer for further reading
						memset(buffer, 0, strlen(buffer));
					}
					//when returns 0 connection was closed by peer and closes as well
					else{
						close(succ_fd);
						server_state.succ_fd = -1;
						reconnection_succ();
					}
				}
			}
			//receive tcp message from predecessor
			if(pred_fd != -1){
				if(FD_ISSET(pred_fd,&read_fds))
				{
					if((n = tcp_read(pred_fd, buffer)) == -1)
						running = false;

					else if (n != 0){
						//handles messages sent from predecessor
						n = message_pred(buffer);
						if (n == -1) running = false;

						//reset the buffer for further reading
						memset(buffer, 0, strlen(buffer));
					}
					//when returns 0 connection was closed by peer and closes as well
					else{
						close(pred_fd);
						server_state.pred_fd = -1;
						flag_pred_out = 1;
					}
				}
			}
			//create tcp connection
			if(fd_server_tcp != -1){
				if(FD_ISSET(fd_server_tcp,&read_fds))
				{	
					addrlen = sizeof(addr);
					incoming_fd = accept(fd_server_tcp,(struct sockaddr*)&addr,&addrlen);
					if(incoming_fd == -1){
						printf("error: cannot read message from incoming fd");
						running = false;
					}
				}
			}
			//receive udp message
			if(FD_ISSET(fd_server_udp,&read_fds))
			{	
				addrlen = sizeof(addr);
 				n = recvfrom(fd_server_udp, buffer, 128, 0, (struct sockaddr*)&addr, &addrlen);
				if(n == -1){
					printf("error: cannot recvfrom\n");
					continue;
				} 
				
				//when EKEY is sent as answer to the previous sendto timer stops 
				if(time_udp != NULL){
					free(time_udp);
					time_udp = NULL;
					time_flag = 0;
				}
				//saves address for later answer
				udp_addr = addr;

				n = message_udp(buffer, udp_addr, IP, port, &waiting_udp, fd_server_udp);
				if (n == -1) running = false;
				if (n == -2) continue;
				
				//reset the buffer for further reading
				memset(buffer, 0, strlen(buffer));
			}
			//resend message if package loss and reconect completed
			if(lost_message.resent == 1 && lost_message.ready == 1){
				decode_fd();
				n = tcp_write(lost_message.fd, lost_message.message);
				if (n > 0){
					lost_message.resent = 0;
					lost_message.ready = 0;
				}
			}
		}
		else{
			if(time_flag == 0){
        		printf("error: No data within 5 seconds message will be resend\n");

				//resends the last udp message sent in case it's lost
				retval = interface_utilizador(last_message, IP, port, &fd_server_udp, &fd_server_tcp);
				
				//waits 5 more seconds
				(*time_udp).tv_sec = 10;
				time_flag = 1;
			}
			else{
				printf("error: No answer received within 10 seconds\n");
				exit(1);
			}
		}
	}

	//closes and free's everything
	close(fd_server_udp);
	if (incoming_fd != -1) close(incoming_fd);
	if (fd_server_udp != -1) close(fd_server_tcp);

	free(IP);
	free(port);
	free(buffer);
	free(last_message);

	if(time_udp != NULL) free(time_udp);

	return 0;
}
