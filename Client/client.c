#include "client.h"

int main(int argc, char *argv[]){

    int fd,errcode, retval, maxfd, key;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints,*res;
    struct sockaddr_in addr;
    char buffer[128];
    fd_set read_fds;
    char IP[128], port[128];

    fd = socket(AF_INET, SOCK_DGRAM, 0); //UDP socket
    if (fd == -1)                       
        exit(1);
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      //IPv4
    hints.ai_socktype = SOCK_DGRAM; //UDP socket

    while(1){
        FD_ZERO(&read_fds);

        FD_SET(STDIN_FILENO,&read_fds);

        FD_SET(fd,&read_fds);

        retval = select(maxfd+1,&read_fds,(fd_set*)NULL,(fd_set*)NULL,(struct timeval *)NULL);
        if(retval <= 0)/*error*/exit(1);

        if(FD_ISSET(STDIN_FILENO,&read_fds)){
            if(fgets(buffer, 128, stdin) == NULL){
                printf("error: cannot read from keyboard\n");
                exit(1);
            }
            n = sscanf(buffer, "%d %s %s", &key, IP, port);
            if(n != 3) exit(1);

            errcode = getaddrinfo(IP, port, &hints, &res);
            if (errcode != 0) exit(1);

            sprintf(buffer, "find %d\n", key);

            n = sendto(fd,buffer,sizeof(buffer),0,res->ai_addr,res->ai_addrlen);
            if(n==-1) exit(1);
        }
    }
    close(fd);
    return 0;
}