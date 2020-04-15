#include "main.h"


int message_incoming_fd(char* message, int incoming_fd, int* flag_pred_out, struct sockaddr_in udp_addr, int fd_server_udp, int *flag_udp){
	char buffer[128], buffer_2[128];
	int search_key, n, fd;
	socklen_t udp_addrlen; 

	if (sscanf(message, "%s", buffer) == 1 && (strcmp(buffer, "NEW")==0)){
        if(sscanf(message, "%s %d %s %s", buffer, &auxiliar.succ_key, auxiliar.succ_IP, auxiliar.succ_TCP) == 4){
			if(server_state.pred_fd == -1){
				server_state.succ_fd = TCP_CLIENT(auxiliar.succ_IP, auxiliar.succ_TCP);
				send_message(server_state.succ_fd, 0, NULL, NULL, "SUCCCONFIG");
				server_state.succ_key = auxiliar.succ_key;
				strcpy(server_state.succ_IP, auxiliar.succ_IP); 
				strcpy(server_state.succ_TCP, auxiliar.succ_TCP);
			}
			else{
                //TCP connection sent do predecessor for update
				send_message(server_state.pred_fd, auxiliar.succ_key, auxiliar.succ_IP, auxiliar.succ_TCP, "NEW");
			}
			//Update self predecessor to new server
			server_state.pred_fd = incoming_fd;
			//TCP connection sent do new server for update
			send_message(incoming_fd, server_state.succ_key, server_state.succ_IP, server_state.succ_TCP, "SUCC");
        }

	}
	else if (sscanf(message, "%s", buffer) == 1 && (strcmp(buffer, "SUCCCONFIG") == 0)){
		server_state.pred_fd = incoming_fd;
		if(*flag_pred_out == 1){
			send_message(incoming_fd, server_state.succ_key, server_state.succ_IP, server_state.succ_TCP, "SUCC");
			*flag_pred_out = 0;
		}
	}
	else if (sscanf(message, "%s", buffer) == 1 && (strcmp(buffer, "KEY") == 0)){
		if(sscanf(message, "%s %d %d %s %s", buffer, &search_key, &auxiliar.node_key, auxiliar.node_IP, auxiliar.node_TCP) == 5 && *flag_udp == 0){
			printf("Key %d is stored on server -> key: %d   IP: %s   Port: %s\n", search_key, auxiliar.node_key, auxiliar.node_IP, auxiliar.node_TCP);
			close(incoming_fd);
		}
		else if(sscanf(message, "%s %d %d %s %s", buffer_2, &search_key, &auxiliar.node_key, auxiliar.node_IP, auxiliar.node_TCP) == 5 && *flag_udp == 1){
			fd = socket(AF_INET, SOCK_DGRAM, 0); //UDP socket
				
			udp_addrlen = sizeof(udp_addr);
			send_message_udp(fd, auxiliar.node_key, auxiliar.node_IP, auxiliar.node_TCP, "EKEY", search_key, (struct sockaddr*)&udp_addr, udp_addrlen);

			close(fd);
			close(incoming_fd);
			*flag_udp = NULL;
		}
		else{
			printf("error: incorrect KEY message\n");
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
	else if (sscanf(message, "%s", buffer) == 1 && (strcmp(buffer, "NEW") == 0)){
		succ_NEW(message);
	}
	else if (sscanf(message, "%s", buffer) == 1 && (strcmp(buffer, "SUCCCONFIG") == 0)){
		server_state.pred_fd = server_state.succ_fd;
	}
	else{
		printf("error: incorrect message from the sucessor\n");
		return -1;
	}

	return 0;
}

int message_pred_fd(char* message){
    char buffer[128];

    if (sscanf(message, "%s", buffer) == 1 &&  strcmp(buffer, "FND") == 0){
		succ_FND(message);
	}
	else{
		printf("error: incorrect message from the predecessor\n");
		return -1;
	}

	return 0;
}

int message_udp(char *message, struct sockaddr_in udp_addr, char *IP, char *port, int *flag_udp){
	char buffer[128];
	int search_key;
	int n, fd;
	socklen_t udp_addrlen;
	struct addrinfo *res;

	if(sscanf(message, "%s", buffer) == 1 && (strcmp(buffer, "EFND") == 0)){
		//checks for correct message
		if(sscanf(message, "%s %d", auxiliar.node_IP, &search_key) == 2){
			//ring with only one server, already has the key
			if (server_state.succ_fd == -1){

				fd = socket(AF_INET, SOCK_DGRAM, 0); //UDP socket

				udp_addrlen = sizeof(udp_addr);
				send_message_udp(fd, server_state.node_key, server_state.node_IP, server_state.node_TCP, "EKEY", search_key, (struct sockaddr*)&udp_addr, udp_addrlen);

				close(fd);
				*flag_udp = 0;
			}
			//sends find message to sucessor
			else if(server_state.succ_fd != -1){
				send_find_message(server_state.succ_fd, server_state.node_key, server_state.node_IP, server_state.node_TCP, "FND", search_key);
			}
		}		
		else{
			printf("error: incorrect message from udp connection\n");
			return -1;
		}
	}
	else if (sscanf(message, "%s %d %d %s %s", buffer, &auxiliar.node_key, &auxiliar.succ_key, auxiliar.succ_IP, auxiliar.succ_TCP) == 5 && strcmp(buffer, "EKEY") == 0){
		if(auxiliar.succ_key != auxiliar.node_key){
			sprintf(buffer, "%s %d %d %s %s\n", "sentry", auxiliar.node_key, auxiliar.succ_key, auxiliar.succ_IP, auxiliar.succ_TCP);
			
			sentry(buffer, IP, port);
		}
		else{
			printf("error: could not perform entry because the key used is already on the ring");
			return -1;
		}
	}
	else{
		printf("error: incorrect udp message\n");
		exit(0); 
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
		close(server_state.succ_fd);
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
		dis_act = server_state.node_key-search_key;
		dis_succ = server_state.succ_key-search_key;
		if(dis_succ < 0) dis_succ += N;
		if(dis_act < 0) dis_act += N;

		if(dis_succ > dis_act){
			send_find_message(server_state.succ_fd, auxiliar.node_key, auxiliar.node_IP, auxiliar.node_TCP, "FND", search_key);
		}
		else{
			auxiliar.succ_fd = TCP_CLIENT(auxiliar.node_IP, auxiliar.node_TCP);
			if(auxiliar.succ_fd != -1){
				send_find_message(auxiliar.succ_fd, server_state.succ_key, server_state.succ_IP, server_state.succ_TCP, "KEY", search_key);
			}
		}
	}
	else{
		printf("error: sscanf did not received the correct FND message\n");
		return -1;
	}	

	return 0;
}

int reconnection_succ(){
	if(server_state.node_key != server_state.succ2_key){
		auxiliar.succ_fd = TCP_CLIENT(server_state.succ2_IP, server_state.succ2_TCP);
		if(auxiliar.succ_fd != -1){	
			server_state.succ_fd = auxiliar.succ_fd;
			send_message(server_state.succ_fd, 0, NULL, NULL, "SUCCCONFIG");
			send_message(server_state.pred_fd, server_state.succ2_key, server_state.succ2_IP, server_state.succ2_TCP, "SUCC");
		}
		else{
			printf("error: TCP connection failed\n");
			return -1;
		}
	}
	
	//update self sucessor with previous second sucessor
	server_state.succ_key = server_state.succ2_key;
	strcpy(server_state.succ_IP, server_state.succ2_IP); 
	strcpy(server_state.succ_TCP, server_state.succ2_TCP);
	
	return 0;
}