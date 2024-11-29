#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>     //  Chứa cấu trúc cần thiết cho socket. 
#include <netinet/in.h>     //  Thư viện chứa các hằng số, cấu trúc khi sử dụng địa chỉ trên internet
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#define BUFF_SIZE 256
#define handle_error(msg) \
    do { \
        perror(msg); \
        exit(EXIT_FAILURE); \
    } while (0)

pthread_t readsensor_thr;

typedef struct{
    int id;
    float temperature;
    float humidity;
    char time[30];
} sensor_node_t;

void get_sensor_time(char *buffer) {
    time_t t = time(NULL);
    struct tm* timeinfo = localtime(&t);
    sprintf(buffer, "%d:%d:%d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}

void simulate_sensor(sensor_node_t* sensor_data, int id) {
    sensor_data->id = id;
    sensor_data->temperature = rand() % 100 + 1;
    sensor_data->humidity    = rand() % 100 + 1;
    get_sensor_time(sensor_data->time);
}

void chat_func(int server_fd){
    int numb_write, numb_read;
    char recvbuff[BUFF_SIZE];
    char sendbuff[BUFF_SIZE];
    sensor_node_t sensor;
    int ret;
    simulate_sensor(&sensor, 1);

    while(1){
        sprintf(sendbuff, "%s - ID: %d\nTemp: %.2f Humid: %.2f ", sensor.time, sensor.id, sensor.temperature, sensor.humidity);
        if(write(server_fd, sendbuff, sizeof(sendbuff)) == -1)
            handle_error("write()"); 

        sleep(1);
    }
}

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
    chat_func(server_fd);
    return 0;
}