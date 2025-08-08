#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "sensor_sender.h"
#include "common.h"

int main(int argc, const char* argv[]) {
    int port_no;
    int server_fd;
    sensor_info_t* sensors[MAX_SENSORS] = {0};
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0,sizeof(serv_addr));

    if (argc < 3) {
        printf("command : ./client <server address> <port number>\n");
        exit(1);
    }
    port_no = atoi(argv[2]);

    serv_addr.sin_family =  AF_INET;
    serv_addr.sin_port   = htons(port_no);
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) == -1) 
        handle_error("inet_pton()");

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(connect(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        handle_error("connect()");
    while(1){
        sensor_send_message(sensors, server_fd);    
        sleep(3);
    }
    for (int i = 0; i < MAX_SENSORS; ++i) {
        free(sensors[i]);
    }
    close(server_fd);
    return 0;
}