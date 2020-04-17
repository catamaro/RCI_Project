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
			//inicialize TCP comunication for 1st server
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
			//inicialize TCP comunication for 1st server
			if(*fd_server_tcp == -1) *fd_server_tcp = TCP_SERVER(port);
			if(*fd_server_tcp == -1) return -1;

			if(sentry(comando_utilizador, IP, port) == 0)
				return -3;
			else
				return -1;
		}
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
		else if(strcmp(buffer, "entry") == 0){
			//check if already on the ring
			if(server_state.node_key != -1){
				printf("error: server already on the ring\n");
				return -1;
			}
			if(*fd_server_tcp == -1) *fd_server_tcp = TCP_SERVER(port);
			if(*fd_server_tcp == -1) return -1;
			
			if(entry(comando_utilizador, IP, port, fd_server_udp) == 0)
				return -6;
			else
				return -1;
		}
		else if(strcmp(buffer, "leave") == 0){
			//check if already on the ring
			if(server_state.node_key == -1){
				printf("error: server is not on the ring\n");
				return -1;
			}
			leave();

			return -7;
		}
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
		send_message(server_state.succ_fd, node_key, IP, port, "NEW");
	}
	else{
		printf("error: command of type sentry <i> <j> <IP_j> <port_j> \n");
		return -1;
	}
	return 0;
}

int find(char* comando_utilizador){

	int search_key;
	char buffer[10];

	if(sscanf(comando_utilizador, "%s %d", buffer, &search_key) == 2){
		if(search_key >= N || search_key < 0){
			printf("error: i cannot overcome %d\n", N);
			return -1;
		}
		
		if(server_state.succ_fd != -1){
			send_find_message(server_state.succ_fd, server_state.node_key, server_state.node_IP, server_state.node_TCP, "FND", search_key);	
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


		res = UDP_CLIENT(auxiliar.succ_IP, auxiliar.succ_TCP, *fd_server_udp);
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

	if(server_state.succ_fd != -1) close(server_state.succ_fd);
	if(server_state.pred_fd != -1) close(server_state.pred_fd);
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

void send_message(int fd, int node_key, char* IP, char* port, char* comand){

	size_t n, nleft, nwritten;
	char message[128], *ptr;

	if(strcmp(comand, "SUCCCONF") == 0) 
		ptr = strcpy(message, "SUCCCONF\n");
	else{
		sprintf(message, "%s %d %s %s\n", comand, node_key, IP, port);
		ptr = message;
	}
	
	n = strlen(message);
	nleft = n;

	while(nleft > 0){
		nwritten = write(fd,ptr,nleft); 
		if(nwritten <= 0){
			printf("error: could not perform write\n");
			exit(1); 
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
}

void send_find_message(int fd, int node_key, char* IP, char* port, char* comand, int search_key){

	size_t n, nleft, nwritten;
	char message[128], *ptr;

	sprintf(message, "%s %d %d %s %s\n", comand, search_key, node_key, IP, port);
	
	n = strlen(message);
	nleft = n;

	while(nleft > 0){
		nwritten = write(fd,message,nleft); 
		if(nwritten <= 0){
			printf("error: could not perform write\n");
			exit(1); 
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
}

void send_message_udp(int fd, int node_key, char* IP, char* port, char* comand, int search_key, struct sockaddr *addr, socklen_t addrlen){
	size_t n;
	char message[128];
	

	if(strcmp(comand, "EFND") == 0){
		sprintf(message, "%s %d\n", comand, node_key);
	}
	else{
		sprintf(message, "%s %d %d %s %s\n", comand, search_key, node_key, IP, port);
	}
		
	n = strlen(message);

	printf("message ups: %s threw fd: %d\n", message, fd);

	n = sendto(fd, message, n, 0, addr, addrlen);
	if(n == -1){
		printf("error: could not perform sendto\n");
	} 
}

int tcp_read(int fd, char* message){
	int n, i = 0;
	char *buffer = (char*) calloc(sizeof(char),128);
	char *aux = (char*) calloc(sizeof(char),128);


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
		
		//no primeiro ciclo copia para o aux para o inicializar e a partir daí concatena o resto da mensagem
		if (i == 0)
			strncpy(aux, buffer, n);
		else
			strncat(aux, buffer, n);		
		i++;

	}while(buffer[n-1] != 10);

	strcpy(message, aux);

	free(aux);
	free(buffer);

	return n;
}
