#include "sensor.h"
sensor_data_t readSensorData(int new_socket_fd) {
    char recvbuff[BUFF_SIZE];
    sensor_data_t recvData;
    memset(recvbuff, '0', BUFF_SIZE);
    if (read(new_socket_fd, recvbuff, BUFF_SIZE) == -1)
            handle_error("read()");
    //Phân tích chuỗi đọc được từ data thành các trường trong recvData
    sscanf(recvbuff, "%[^-] - ID: %d\nTemp: %f\n", recvData.time, 
                                                   &recvData.id, 
                                                   &recvData.temperature);
    return recvData;
}

void sendSensorData(int server_fd){
    char sendbuff[BUFF_SIZE];
    sensor_data_t sensor;
    int id = 6;
    while(id != 0){
        memset(&sendbuff, '0', BUFF_SIZE);
        simulate_sensor(&sensor, id);
        sprintf(sendbuff, "%s - ID: %d\nTemp: %.2f\n", sensor.time, sensor.id, sensor.temperature);
        if(write(server_fd, sendbuff, strlen(sendbuff)) == -1)
            handle_error("write()"); 
        printf("%s", sendbuff);
        id--;
        sleep(1);
    }
}

void get_time(char *buffer) {
    time_t t = time(NULL);
    struct tm* timeinfo = localtime(&t);
    sprintf(buffer, "%d:%d:%d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}

void simulate_sensor(sensor_data_t* sensor_data, int id) {
    srand(time(NULL));
    sensor_data->id = id;
    sensor_data->temperature = rand() % 100 + 1;
    get_time(sensor_data->time);
}


