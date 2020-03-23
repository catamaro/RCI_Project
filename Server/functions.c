#include "connect.h"

int message_incoming_fd(char* message, int incoming_fd){
	char buffer[128];
	int search_key;
	
	if (sscanf(message, "%s", buffer) == 1 && (strcmp(buffer, "NEW")==0)){
		//caso em que só está um server no anel, o sucessor do server que está no anel é o server que vai entrar no anel
		if(server_state.pred_fd == -1){ 
			if(sscanf(message, "%s %d %s %s", buffer, &server_state.succ_key, server_state.succ_IP, server_state.succ_TCP) == 4){
				send_message(incoming_fd, server_state.succ_key, server_state.succ_IP, server_state.succ_TCP, "SUCC");
				send_message(incoming_fd, 0, NULL, NULL, "SUCCCONFIG");
				server_state.succ_fd = incoming_fd;
                server_state.pred_fd = incoming_fd;
			}
			else 
				printf("error: invalid message received\n");
		}
		else{ 
            if(sscanf(message, "%s %d %s %s", buffer, &auxiliar.succ_key, auxiliar.succ_IP, auxiliar.succ_TCP) == 4){
                //TCP connection sent do predecessor for update
				send_message(server_state.pred_fd, auxiliar.succ_key, auxiliar.succ_IP, auxiliar.succ_TCP, "NEW");
				//Update self predecessor to new server
                server_state.pred_fd = incoming_fd;
				//TCP connection sent do new server for update
				send_message(incoming_fd, server_state.succ_key, server_state.succ_IP, server_state.succ_TCP, "SUCC");
            }
        }
	}
	else if (sscanf(message, "%s", buffer) == 1 && (strcmp(buffer, "SUCCCONFIG")==0)){
		server_state.pred_fd = incoming_fd;
	}
	else if (sscanf(message, "%s", buffer) == 1 && (strcmp(buffer, "KEY") == 0)){
		if(sscanf(message, "%s %d %d %s %s", buffer, &search_key, &auxiliar.node_key, auxiliar.node_IP, auxiliar.node_TCP) == 5){
			printf("Key %d is stored on server -> key: %d   IP: %s   Port: %s\n", search_key, auxiliar.node_key, auxiliar.node_IP, auxiliar.node_TCP);
		}
	}
	else {
		printf("error: incorrect message from new server\n");
		return -1;
	}
	return 0;
}

int message_succ_fd(char* message){
    char buffer[128];

    if (sscanf(message, "%s", buffer) == 1 && (strcmp(buffer, "SUCC") == 0)){
		succ_SUCC(message);
	}
	else if (sscanf(message, "%s", buffer) == 1 && (strcmp(buffer, "NEW")==0)){
		succ_NEW(message);
	}
	else if (sscanf(message, "%s", buffer) == 1 && (strcmp(buffer, "SUCCCONFIG") == 0)){
		server_state.pred_fd = server_state.succ_fd;
	}
	else if (sscanf(message, "%s", buffer) == 1 && (strcmp(buffer, "FND") == 0)){
		succ_FND(message);
	}
	else{
		printf("error: incorrect message from the sucessor\n");
		return -1;
	}

	return 0;
}

int message_pred_fd(char* message){
    char buffer[128];
	
    if (sscanf(message, "%s", buffer) == 1 && (strcmp(buffer, "FND") == 0)){
		succ_FND(message);
	}
	else{
		printf("error: incorrect message from the predecessor\n");
		return -1;
	}

	return 0;
}

int succ_SUCC(char* message){
	char buffer[128];

	//check if SUCC has all the parameters
	if(sscanf(message, "%s %d %s %s", buffer, &auxiliar.succ_key, auxiliar.succ_IP, auxiliar.succ_TCP) == 4){
		//update second sucessor info
		server_state.succ2_key = auxiliar.succ_key;
		strcpy(server_state.succ2_IP, auxiliar.succ_IP); 
		strcpy(server_state.succ2_TCP, auxiliar.succ_TCP);
	}
	else{
		printf("error: sscanf did not received the correct SUCC message\n");
		return -1;
	}	

	return 0;
}

int succ_NEW(char* message){
	char buffer[128];

	//check if NEW has all the parameters
	if(sscanf(message, "%s %d %s %s", buffer, &auxiliar.succ_key, auxiliar.succ_IP, auxiliar.succ_TCP) == 4){
		//close connection with sucessor if there is more than two servers
		if(server_state.pred_fd != server_state.succ_fd){ 
			close(server_state.succ_fd);
		}
		//TCP connection with new sucessor
		auxiliar.succ_fd = TCP_CLIENT(auxiliar.succ_IP, auxiliar.succ_TCP);
		if(auxiliar.succ_fd != -1){
			server_state.succ_fd = auxiliar.succ_fd;
			//update self second sucessor with previous sucessor
			server_state.succ2_key = server_state.succ_key;
			strcpy(server_state.succ2_IP, server_state.succ_IP); 
			strcpy(server_state.succ2_TCP, server_state.succ_TCP);
			//update self with new sucessor
			server_state.succ_key = auxiliar.succ_key;
			strcpy(server_state.succ_IP, auxiliar.succ_IP); 
			strcpy(server_state.succ_TCP, auxiliar.succ_TCP);

			send_message(server_state.succ_fd, 0, NULL, NULL, "SUCCCONFIG");
			send_message(server_state.pred_fd, server_state.succ_key, server_state.succ_IP, server_state.succ_TCP, "SUCC");
		}            
	}
	else{
		printf("error: sscanf did not received the correct NEW message\n");
		return -1;
	}

	return 0;	
}

int succ_FND(char* message){
	char buffer[128];
	int search_key, dis_act, dis_succ;
	
	if(sscanf(message, "%s %d %d %s %s", buffer, &search_key, &auxiliar.node_key, auxiliar.node_IP, auxiliar.node_TCP) == 5){
		dis_act = N-search_key+server_state.node_key;
		dis_succ = N-search_key+server_state.succ_key;
		if(dis_succ > N) dis_succ -= 32;

		if(dis_succ > dis_act){
			send_find_message(server_state.succ_fd, auxiliar.node_key, auxiliar.node_IP, auxiliar.node_TCP, "FND", search_key);
		}
		else{
			auxiliar.succ_fd = TCP_CLIENT(auxiliar.node_IP, auxiliar.node_TCP);
			if(auxiliar.succ_fd != -1) 
				send_find_message(auxiliar.succ_fd, server_state.succ_key, server_state.succ_IP, server_state.succ_TCP, "KEY", search_key);
		}
	}
	else{
		printf("error: sscanf did not received the correct FND message\n");
		return -1;
	}	

	return 0;
}