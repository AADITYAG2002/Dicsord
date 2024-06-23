#ifndef _LIB_CLIENT_BE_H_
#define _LIB_CLIENT_BE_H_

#include <arpa/inet.h>

struct CLIENT {
    int status;
    int client_fd;
    struct sockaddr_in serv_addr;
};

int initialize(struct CLIENT *client, char *ip_addr, int port);
int transmit(struct CLIENT *client, char *buf);
int receive(struct CLIENT *client, char *buf, int BUFLEN);
void terminate(struct CLIENT *client);

#endif
