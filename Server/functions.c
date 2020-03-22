#include "connect.h"

int message_incoming_fd(char* buffer, int incoming_fd){
	char aux[128];

	if (sscanf(buffer, "%s", aux) == 1 && (strcmp(aux, "NEW")==0)){
		//caso em que só está um server no anel, o sucessor do server que está no anel é o server que vai entrar no anel
		if(server_state.pred_fd == -1){ 
			if(sscanf(buffer, "%s %d %s %s", aux, &server_state.succ_key, server_state.succ_IP, server_state.succ_TCP) == 4){
                //
				SUCC(incoming_fd, server_state.succ_key, server_state.succ_IP, server_state.succ_TCP);
				SUCCCONFIG(incoming_fd);
				server_state.succ_fd = incoming_fd;
                server_state.pred_fd = incoming_fd;
			}
			else 
				printf("error: invalid message received\n");
		}
		else{ 
            if(sscanf(buffer, "%s %d %s %s", aux, &auxiliar.succ_key, auxiliar.succ_IP, auxiliar.succ_TCP) == 4){
                //TCP connection sent do predecessor for update
				NEW(server_state.pred_fd, auxiliar.succ_key, auxiliar.succ_IP, auxiliar.succ_TCP);
				//Update self predecessor to new server
                server_state.pred_fd = incoming_fd;
				//TCP connection sent do new server for update
			    SUCC(incoming_fd, server_state.succ_key, server_state.succ_IP, server_state.succ_TCP);
            }
        }
	}
	else if (sscanf(buffer, "%s", aux) == 1 && (strcmp(aux, "SUCCCONFIG")==0)){
		server_state.pred_fd = incoming_fd;
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
	}
	else if (sscanf(message, "%s", buffer) == 1 && (strcmp(buffer, "NEW")==0)){
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
				//send
				SUCCCONFIG(server_state.succ_fd);
				SUCC(server_state.pred_fd, server_state.succ_key, server_state.succ_IP, server_state.succ_TCP);
			}            
		}
		else{
			printf("error: sscanf did not received the correct NEW message\n");
			return -1;
		}	
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

/*void message_pred_fd(char* buffer){
    int n;
    char aux[128];
    
    if (sscanf(buffer, "%s", aux) == 1 && (strcmp(aux, "NEW")==0)){
        n = strlen(buffer);
		n = write(server_state.pred_fd, buffer, n);
		SUCC(incoming_fd, server_state.succ_key, server_state.succ_IP, server_state.succ_TCP);
		server_state.pred_fd = incoming_fd;
	}
    else{
        printf("Erro no sscanf da mensagem vinda do meu predecessor\n");
    }
}*/