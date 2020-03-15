#include "connect.h"

int interface_utilizador(char* comando_utilizador, char* IP, char* port){

	char buffer[1024];

    if (sscanf(comando_utilizador, "%s", buffer) == 1){
		
		//create ring if "new i" is typed
		if(strcmp(buffer, "new") == 0){
			//check if already on the ring
			if(server_state.node_key != -1){
				printf("error: before the command <new> use the command <leave> to exit the current ring\n");
				return 1;
			}
			new(comando_utilizador, IP, port);
		}

		//add new server without search
		else if(strcmp(buffer, "sentry") == 0){
			//check if already on the ring
			if(server_state.node_key != -1){
				printf("error: server already on the ring\n");
				return 1;
			}
			//loading...
		}

		//add new server with search
		else if(strcmp(buffer, "entry") == 0){
			//check if already on the ring
			if(server_state.node_key != -1){
				printf("error: server already on the ring\n");
				return 1;
			}
			//loading...
		}

		else if(strcmp(buffer, "leave") == 0){
			//loading...
		}

		else if(strcmp(buffer, "show") == 0){
			//loading...
		}

		else if(strcmp(buffer, "find") == 0){
			//loading...
		}

		else if(strcmp(buffer, "exit") == 0){
			//loading...
		}

		else{
			printf("error: invalid input\n");
			return 1;
		}
	}
	return 0;
}

void new(char* comando_utilizador, char* IP, char* port){
	int node_key;
	char buffer[1024];

	//initiate ring with 1st server info 
	if (sscanf(comando_utilizador, "%s %d", buffer, &node_key) == 2){
		if(node_key > 32){
			printf("i cannot overcome %d\n", N);
			return;
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
}
