#include "socket.h"

int socketInit(int server_fd, struct sockaddr_in serv_addr, int opt) {
    /* Khởi tạo socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
            handle_error("socket()");

    // Ngăn lỗi: “address already in use”
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
        handle_error("setsockopt()");
    if (bind(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        handle_error("bind()");
    return server_fd;
}

int connectSocket(int server_fd, int port) {
    struct sockaddr_in clientAddr;
    if (listen(server_fd, 5) == -1)
        handle_error("listen()");
    socklen_t clientLen  = sizeof(clientAddr);
    printf("Server is listening at port : %d \n....\n",port);
    int new_socket_fd = accept(server_fd, (struct sockaddr*)&clientAddr, &clientLen);
    if (new_socket_fd == -1)
        handle_error("accept()");
    printf("Connected");
    return new_socket_fd;
}