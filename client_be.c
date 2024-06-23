#include <bits/types/struct_timeval.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "client_be.h"

enum RETURN {
    ERR = -1,
    OK = 0 
};

int initialize(struct CLIENT *client, char *ip_addr, int port){
    if((client->client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("\n Socket creation error\n");
        return ERR;
    }

    client->serv_addr.sin_family = AF_INET;
    client->serv_addr.sin_port = htons(port);

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 1000;
    setsockopt(client->client_fd, SOL_SOCKET, SO_RCVTIMEO,(const char*)&tv ,sizeof(tv));

    if (inet_pton(AF_INET, ip_addr, &client->serv_addr.sin_addr) <= 0){
        printf("\n Invalid address / Address not supported \n");
        return ERR;
    }

    if((client->status = connect(client->client_fd, (struct sockaddr*)&client->serv_addr, sizeof(client->serv_addr))) < 0){
        printf("\nConnection Failed\n");
        return ERR;
    }
    return OK;
}

int transmit(struct CLIENT *client, char *buf){
    if(send(client->client_fd, buf, strlen(buf), 0) < 0){
        return ERR;
    }
    return OK;
}

int receive(struct CLIENT *client, char *buf, int BUFLEN){
    int read_size = recv(client->client_fd, buf, BUFLEN, 0);
    // buf[read_size - 1] = '\0';

    return read_size;
}

void terminate(struct CLIENT *client){
    close(client->client_fd);
    return;
}
