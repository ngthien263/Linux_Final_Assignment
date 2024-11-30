#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>     //  Chứa cấu trúc cần thiết cho socket. 
#include <netinet/in.h>     //  Thư viện chứa các hằng số, cấu trúc khi sử dụng địa chỉ trên internet
#include <arpa/inet.h>
#include "log.h"
int socketInit(int server_fd, struct sockaddr_in serv_addr, int opt);
int connectSocket(int server_fd, int port);
#endif