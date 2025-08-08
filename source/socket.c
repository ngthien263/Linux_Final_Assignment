#include "socket.h"
#include "sensor_types.h"
#include "common.h"

int socket_init(socket_t* sock_config) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &sock_config->opt, sizeof(sock_config->opt)) == -1)
        handle_error("setsockopt()");
    if (bind(server_fd, (struct sockaddr*)&sock_config->serv_addr, sizeof(sock_config->serv_addr)) == -1)
        handle_error("bind()");
    return server_fd;
}

int socket_bind_listen(socket_t* sock_config) {
    int server_fd = socket_init(sock_config);
    if(listen(server_fd, MAX_SENSORS) < 0)
        handle_error("listen()");
    printf("Server is listening at port: %d \n....\n", sock_config->port);
    return server_fd;
}

int socket_connect(socket_t* sock_config) {
    int server_fd = socket_bind_listen(sock_config);
    socklen_t addrlen = sizeof(sock_config->client_addr);
    int newsocket_fd = accept(server_fd, (struct sockaddr*)&sock_config->client_addr, &addrlen);
    if(newsocket_fd < 0)
        handle_error("accept()");
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &sock_config->client_addr.sin_addr, ip, sizeof ip);
    printf("Connected to client %s:%d\n", ip, ntohs(sock_config->client_addr.sin_port));
    return newsocket_fd;
}
