#ifndef SOCKET_H
#define SOCKET_H

#include <arpa/inet.h>

typedef struct {
    int port;
    int opt;
    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;
} socket_t;

int socket_init(socket_t* socket);
int socket_bind_listen(socket_t* socket);
int socket_connect(socket_t* socket);
#endif