#include "connect.h"

int interface_utilizador(char* comando_utilizador, char* IP, char* port){
	char buffer[128];
	int search_key;

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
			//loading...
		}
		else if(strcmp(buffer, "leave") == 0){
			//check if already on the ring
			if(server_state.node_key == -1){
				printf("error: server is not on the ring\n");
				return -1;
			}
			//loading...
		}
		else if(strcmp(buffer, "exit") == 0){
			if(server_state.node_key == -1){
				printf("error: server is not on the ring\n");
				return -1;
			}
			//loading...
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

	int search_key, dis_act, dis_succ;
	char buffer[10];

	if(sscanf(comando_utilizador, "%s %d", buffer, &search_key) == 2){
		if(search_key > N){
			printf("i cannot overcome %d\n", N);
			return -1;
		}

		dis_act = N-search_key+server_state.node_key;
		dis_succ = N-search_key+server_state.succ_key;
		if(dis_succ > N) dis_succ -= 32;
		
		if(dis_succ > dis_act){
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

void send_message(int fd, int node_key, char* IP, char* port, char* comand){

	size_t n;
	char message[128];

	if(strcmp(comand, "SUCCCONFIG") == 0) 
		strcpy(message, "SUCCCONFIG\n");
	else
		sprintf(message, "%s %d %s %s\n", comand, node_key, IP, port);
	
	n = strlen(message);
	
	n = write(fd,message,n);
	if(n == -1)exit(1);
}

void send_find_message(int fd, int node_key, char* IP, char* port, char* comand, int search_key){

	size_t n;
	char message[128];

	sprintf(message, "%s %d %d %s %s\n", comand, search_key, node_key, IP, port);
	n = strlen(message);

	printf("mensagem: %s\n", message);
	
	n = write(fd, message, n);
	if(n == -1)exit(1);
}


