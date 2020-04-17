#include "main.h"

int interface_utilizador(char* comando_utilizador, char* IP, char* port, int *fd_server_udp, int *fd_server_tcp){
	char buffer[128];

    if(sscanf(comando_utilizador, "%s", buffer) == 1){
		
		//create ring if "new i" is typed
		if(strcmp(buffer, "new") == 0){
			//check if already on the ring
			if(server_state.node_key != -1){
				printf("error: before the command <new> use the command <leave> to exit the current ring\n");
				return -1;
			}
			//inicialize TCP comunication when entering the ring			
			*fd_server_tcp = TCP_SERVER(port);
			if(*fd_server_tcp == -1) return -1;

			if(new(comando_utilizador, IP, port) == 0)
				return -2;
			else 
				return -1;
			
		}
		//add new server without search
		else if(strcmp(buffer, "sentry") == 0){
			//check if already on the ring
			if(server_state.node_key != -1){
				printf("error: server already on the ring\n");
				return -1;
			}
			//inicialize TCP comunication when entering the ring
			if(*fd_server_tcp == -1) *fd_server_tcp = TCP_SERVER(port);
			if(*fd_server_tcp == -1) return -1;

			if(sentry(comando_utilizador, IP, port) == 0)
				return -3;
			else
				return -1;
		}
		//show information stored on the ring
		else if(strcmp(buffer, "show") == 0){
			//check if already on the ring
			if(server_state.node_key != -1){
				printf("\nServer state:\n");
				printf("Server -> key: %d   IP: %s   Port: %s\n", server_state.node_key, server_state.node_IP, server_state.node_TCP);
				printf("Sucessor -> key: %d   IP: %s   Port: %s\n", server_state.succ_key, server_state.succ_IP, server_state.succ_TCP);
				printf("Second sucessor -> key: %d   IP: %s   Port: %s\n", server_state.succ2_key, server_state.succ2_IP, server_state.succ2_TCP);
				return -4;
			}
			else{
				printf("error: server not on the ring type <new>, <entry> or <sentry>\n");
				return -1;
			}
		}
		//send find message for sucessor
		else if(strcmp(buffer, "find") == 0){
			//check if already on the ring
			if(server_state.node_key == -1){
				printf("error: server is not on the ring\n");
				return -1;
			}
			else if(find(comando_utilizador) == 0)
				return -5;
			else
				return -1;
		}
		//entry on the ring with search of sucessor
		else if(strcmp(buffer, "entry") == 0){
			//check if already on the ring
			if(server_state.node_key != -1){
				printf("error: server already on the ring\n");
				return -1;
			}
			//inicialize TCP comunication when entering the ring	
			if(*fd_server_tcp == -1) *fd_server_tcp = TCP_SERVER(port);
			if(*fd_server_tcp == -1) return -1;
			
			if(entry(comando_utilizador, IP, port, fd_server_udp) == 0)
				return -6;
			else
				return -1;
		}
		//resets ring information and closes fd
		else if(strcmp(buffer, "leave") == 0){
			//check if already on the ring
			if(server_state.node_key == -1){
				printf("error: server is not on the ring\n");
				return -1;
			}
			leave();

			return -7;
		}
		//resets ring information and closes fd
		else if(strcmp(buffer, "exit") == 0){
			if(server_state.node_key == -1)
				return -8;

			leave();
				
			return -8;
		}
		else{
			printf("error: invalid input\n");
			return -1;
		}
	}
	else{
		printf("error: could not read the comand\n");
		return -1;
	}
}

int new(char* comando_utilizador, char* IP, char* port){
	int node_key;
	char buffer[10];

	//initiate ring with 1st server info 
	if (sscanf(comando_utilizador, "%s %d", buffer, &node_key) == 2){
		
		if(node_key >= 32 || node_key < 0){
			printf("error: i cannot overcome %d\n", N);
			return -1;
		}
				
		//add information to create the ring
		server_state.node_key = node_key; 
		strcpy(server_state.node_IP, IP); 
		strcpy(server_state.node_TCP, port); 
		server_state.succ_key = node_key; 
		strcpy(server_state.succ_IP, IP); 
		strcpy(server_state.succ_TCP, port); 
		server_state.succ2_key = node_key; 
		strcpy(server_state.succ2_IP, IP); 
		strcpy(server_state.succ2_TCP, port);
	}
	else{
		printf("error: command of type new <i>\n");
		return -1;
	}
	return 0;
}

int sentry(char* comando_utilizador, char* IP, char* port){
	int node_key;
	char buffer[10];

	if (sscanf(comando_utilizador, "%s %d %d %s %s", buffer, &node_key, &server_state.succ_key, server_state.succ_IP, server_state.succ_TCP) == 5){
		if(node_key >= N || node_key < 0){
			printf("error: i cannot overcome %d\n", N);
			return -1;
		}
		else if(server_state.succ_key >= N || server_state.succ_key < 0){
			printf("error: succ_key cannot overcome %d\n", N);
			return -1;
		}
		//state of new server is updated
		server_state.node_key = node_key; 
		strcpy(server_state.node_IP, IP); 
		strcpy(server_state.node_TCP, port);
		
		//TCP connection created with sucessor
		server_state.succ_fd = TCP_CLIENT(server_state.succ_IP, server_state.succ_TCP);
		
		//TCP connection sent do sucessor for update
		send_message_tcp(server_state.succ_fd, node_key, IP, port, "NEW", 0);
	}
	else{
		printf("error: command of type sentry <i> <j> <IP_j> <port_j> \n");
		return -1;
	}
	return 0;
}

int find(char* comando_utilizador){
	size_t n;
	int search_key;
	char buffer[10];

	if(sscanf(comando_utilizador, "%s %d", buffer, &search_key) == 2){
		if(search_key >= N || search_key < 0){
			printf("error: i cannot overcome %d\n", N);
			return -1;
		}
		//if not alone on the ring send find message to sucessor
		if(server_state.succ_fd != -1){
			n = send_message_tcp(server_state.succ_fd, server_state.node_key, server_state.node_IP, server_state.node_TCP, "FND", search_key);	
			//when write fails the message is kept, the name of the veriable where the message is to be sent
			//and the flag is activated
			if(n == -1){
				strcpy(lost_message.encode_fd, "server_state.succ_fd");
				lost_message.resent = 1;
				return -1;
			} 
		}
		else{
			printf("Key %d is stored on server -> key: %d   IP: %s   Port: %s\n", search_key, server_state.node_key, server_state.node_IP, server_state.node_TCP);
		}
	}
	else{
		printf("error: command of type find <i>\n");
		return -1;
	}
	return 0;
}

int entry(char* comando_utilizador, char* IP, char* port, int *fd_server_udp){
	int node_key;
	struct addrinfo *res;
	char buffer[128];

	if (sscanf(comando_utilizador, "%s %d %d %s %s", buffer, &node_key, &auxiliar.succ_key, auxiliar.succ_IP, auxiliar.succ_TCP) == 5){
		if(node_key >= N || node_key < 0){
			printf("i cannot overcome %d\n", N);
			return -1;
		}
		else if(auxiliar.succ_key >= N || auxiliar.succ_key < 0){
			printf("succ_key cannot overcome %d\n", N);
			return -1;
		}


		//gets the address of the server where EFND is going to be sent
		res = UDP_CLIENT(auxiliar.succ_IP, auxiliar.succ_TCP, *fd_server_udp);
		//sends udp message 
		send_message_udp(*fd_server_udp, node_key, NULL, NULL, "EFND", 0, (struct sockaddr *)res->ai_addr, res->ai_addrlen);
		
		freeaddrinfo(res);
	}
	else{
		printf("error: command of type sentry <i> <j> <IP_j> <port_j> \n");
		return -1;
	}

	return 0;
}

void leave(){

	//reset of all the server state information
	if(server_state.succ_fd != -1) close(server_state.succ_fd);
	if(server_state.pred_fd != -1) close(server_state.pred_fd); //shutdown(server_state.pred_fd, SHUT_RDWR);
	server_state.succ_fd = -1;
	server_state.pred_fd = -1;
	server_state.node_key = -1; 
	server_state.succ_key = -1; 
	strcpy(server_state.succ_IP, "0000"); 
	strcpy(server_state.succ_TCP, "0000"); 
	server_state.succ2_key = -1; 
	strcpy(server_state.succ2_IP, "0000"); 
	strcpy(server_state.succ2_TCP, "0000");
}

int send_message_tcp(int fd, int node_key, char* IP, char* port, char* comand, int search_key){

	size_t n;
	char message[128];

	//construct all the possible tcp messages
	if(strcmp(comand, "SUCCCONF") == 0) 
		strcpy(message, "SUCCCONF\n");
	else if(strcmp(comand, "KEY") == 0 || strcmp(comand, "FND") == 0)
		sprintf(message, "%s %d %d %s %s\n", comand, search_key, node_key, IP, port);
	else if(strcmp(comand, "NEW") == 0 || strcmp(comand, "SUCC") == 0)
		sprintf(message, "%s %d %s %s\n", comand, node_key, IP, port);
	else return -1;

	//send tcp message 
	n = tcp_write(fd, message);
	if(n == -1) return -1;

	return 0;
}

void send_message_udp(int fd, int node_key, char* IP, char* port, char* comand, int search_key, struct sockaddr *addr, socklen_t addrlen){
	size_t n;
	char message[128];
	
	//construct all the possible udp messages
	if(strcmp(comand, "EFND") == 0)
		sprintf(message, "%s %d\n", comand, node_key);
	else if(strcmp(comand, "EKEY") == 0)
		sprintf(message, "%s %d %d %s %s\n", comand, search_key, node_key, IP, port);
	else return;
		
	n = strlen(message);

	//if message is not sent timer of server who sent the
	//message will expire and send it again, if incorrect
	//once again sender aplication will end
	n = sendto(fd, message, n, 0, addr, addrlen);
	if(n == -1){
		printf("error: could not perform sendto\n");
	} 
}

int tcp_read(int fd, char* message){
	int n, i = 0;
	char *buffer = (char*) calloc(sizeof(char),128);
	char *aux = (char*) calloc(sizeof(char),128);

	//read message package by package
	do{
		//reset the buffer for further reading
		memset(buffer, 0, strlen(buffer));

		if((n = read(fd, buffer, 128)) !=0 ){
			if(n == -1){
				printf("error: cannot read message from this descriptor %d\n", fd);
				return -1;
			}
		}
		else return n;
		
		//copy buffer to aux in the first iteration
		if (i == 0)
			strncpy(aux, buffer, n);
		else
		//construct the message byte by byte
			strncat(aux, buffer, n);		
		i++;

	}while(buffer[n-1] != 10);//stops when \n is encountered, \n = 10 in ascii

	//final message is stored
	strcpy(message, aux);

	free(aux);
	free(buffer);

	return n;
}

int tcp_write(int fd, char* message){
	size_t n, nleft, nwritten;
	char *ptr;

	ptr = message;

	n = strlen(message);
	nleft = n;

	//write message package by package for further reading
	while(nleft > 0){
		nwritten = write(fd,message,nleft); 
		if((int)nwritten <= 0){
			strcpy(lost_message.message, message);
			printf("error: could not perform write\n");
			return -1; 
		}
		nleft -= nwritten;
		ptr += nwritten;
	}

	return n;
}