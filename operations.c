#include "connect.h"

void interface_utilizador(char* comando_utilizador, char* IP, char* port){

	char buffer[1024];
	int node_key;

    if (sscanf(comando_utilizador, "%s", buffer) == 1){
		if (strcmp(buffer, "new") == 0){
			if (sscanf(comando_utilizador, "%s %d", buffer, &node_key) == 2){
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
	}
}