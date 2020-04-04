#include "main.h"

int interface_utilizador(char* comando_utilizador, char* IP, char* port){
	char buffer[128];

    if(sscanf(comando_utilizador, "%s", buffer) == 1){
		
		//create ring if "new i" is typed
		if(strcmp(buffer, "new") == 0){
			//check if already on the ring
			if(server_state.node_key != -1){
				printf("error: before the command <new> use the command <leave> to exit the current ring\n");
				return -1;
			}
			else if(new(comando_utilizador, IP, port) == 0)
				return -2;
			else{
				printf("error: cannot perform <new>\n");
				return -1;
			}
		}

		//add new server without search
		else if(strcmp(buffer, "sentry") == 0){
			//check if already on the ring
			if(server_state.node_key != -1){
				printf("error: server already on the ring\n");
				return -1;
			}
			else if(sentry(comando_utilizador, IP, port) == 0){
				return -3;
			}
			else{
				printf("error: cannot perform <sentry>\n");
				return -1;
			}
			
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
			else{
				printf("error: cannot perform <find>\n");
				return -1;
			}
		}
		else if(strcmp(buffer, "entry") == 0){
			//check if already on the ring
			if(server_state.node_key != -1){
				printf("error: server already on the ring\n");
				return -1;
			}
			else if(entry(comando_utilizador, IP, port) == 0){
				return -6;
			}
			else{
				printf("error: cannot perform <entry>\n");
				return -1;
			}
		}
		else if(strcmp(buffer, "leave") == 0){
			//check if already on the ring
			if(server_state.node_key == -1){
				printf("error: server is not on the ring\n");
				return -1;
			}
			else if(leave() == 0){
				return -7;
			}
			else{
				printf("error: cannot perform <leave>\n");
				return -1;
			}			
		}
		else if(strcmp(buffer, "exit") == 0){
			if(server_state.node_key == -1){
				return -8;
			}
			else if(leave() == 0){
				return -8;
			}
			else{
				printf("error: cannot perform <exit>\n");
				return -1;
			}
		}
		else{
			printf("error: invalid input\n");
			return -1;
		}
	}

	return -1;
}

int new(char* comando_utilizador, char* IP, char* port){
	int node_key;
	char buffer[10];

	//initiate ring with 1st server info 
	if (sscanf(comando_utilizador, "%s %d", buffer, &node_key) == 2){
		
		if(node_key > 32){
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
		if(node_key > 32){
			printf("i cannot overcome %d\n", N);
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
		if(search_key > N){
			printf("error: i cannot overcome %d\n", N);
			return -1;
		}
		
		if(server_state.succ_fd != -1){
			send_find_message(server_state.succ_fd, server_state.node_key, server_state.node_IP, server_state.node_TCP, "FND", search_key);	
		}
		else{
			printf("error: there's no successor\n");
		}

	}
	else{
		printf("error: command of type find <i>\n");
		return -1;
	}
	return 0;
}

int entry(char* comando_utilizador, char* IP, char* port){
	int node_key, fd = 0;
	struct addrinfo *res;
	char buffer[128];
	char message[128];

	if (sscanf(comando_utilizador, "%s %d %d %s %s", buffer, &node_key, &auxiliar.succ_key, auxiliar.succ_IP, auxiliar.succ_TCP) == 5){
		if(node_key > 32){
			printf("i cannot overcome %d\n", N);
			return -1;
		}

		res = UDP_CLIENT(auxiliar.succ_IP, auxiliar.succ_TCP, &fd);
		send_message_udp(fd, node_key, res, NULL, NULL, "EFND", 0);
		close(fd);

		/*recvfrom(fd, message, 128, 0, res->ai_addr, &(res->ai_addrlen));
		*/
	}
	else{
		printf("error: command of type sentry <i> <j> <IP_j> <port_j> \n");
		return -1;
	}

	/*if (sscanf(message, "%s %d %d %s %s", buffer, &node_key, &auxiliar.succ_key, auxiliar.succ_IP, auxiliar.succ_TCP) == 5){
		sprintf(message, "%s %d %d %s %s\n", "sentry", node_key, auxiliar.succ_key, auxiliar.succ_IP, auxiliar.succ_TCP);
		sentry(message, IP, port);
	}
	else{
		printf("error: command of type EKEY <i> <j> <IP_j> <port_j> \n");
		return -1;
	}*/

	return 0;
}

int leave(){

	close(server_state.succ_fd);
	close(server_state.pred_fd);
	server_state.succ_fd = -1;
	server_state.pred_fd = -1;
	server_state.node_key = -1; 
	server_state.succ_key = -1; 
	strcpy(server_state.succ_IP, "Not"); 
	strcpy(server_state.succ_TCP, "Not"); 
	server_state.succ2_key = -1; 
	strcpy(server_state.succ2_IP, "Not"); 
	strcpy(server_state.succ2_TCP, "Not");

	return 0;
}

void send_message(int fd, int node_key, char* IP, char* port, char* comand){

	size_t n;
	char message[128];

	if(strcmp(comand, "SUCCCONFIG") == 0) 
		strcpy(message, "SUCCCONFIG\n");
	else
		sprintf(message, "%s %d %s %s\n", comand, node_key, IP, port);
	
	n = strlen(message);
	
	n = write(fd,message,n);
	if(n == -1) exit(1);
}

void send_find_message(int fd, int node_key, char* IP, char* port, char* comand, int search_key){

	size_t n;
	char message[128];

	sprintf(message, "%s %d %d %s %s\n", comand, search_key, node_key, IP, port);
	n = strlen(message);

	n = write(fd, message, n);
	if(n == -1)exit(1);
}

void send_message_udp(int fd, int node_key, struct addrinfo *res, char* IP, char* port, char* comand, int search_key){
	size_t n;
	char message[128];

	if(strcmp(comand, "EFND") == 0) 
		sprintf(message, "%s %d\n", comand, node_key);
	else
		sprintf(message, "%s %d %d %s %s\n", comand, search_key, node_key, IP, port);

	printf("message: %s\n", message);
	n = strlen(message);

	n = sendto(fd,message,n,0,res->ai_addr,res->ai_addrlen);
	if(n==-1) exit(1);
}


