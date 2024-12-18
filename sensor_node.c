#include "sensor.h"

int main(int argc, const char* argv[]) {
    int port_no;
    int server_fd;
    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0',sizeof(serv_addr));
    /* Đọc portnumber từ command line */
    if (argc < 3) {
        printf("command : ./client <server address> <port number>\n");
        exit(1);
    }
    port_no = atoi(argv[2]);

    serv_addr.sin_family =  AF_INET;
    serv_addr.sin_port   = htons(port_no);
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) == -1) 
        handle_error("inet_pton()");

    /*Tạo socket*/
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(connect(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        handle_error("connect()");
    sendSensorData(server_fd);
    return 0;
}