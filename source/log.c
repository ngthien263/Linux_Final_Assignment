#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "log.h"

void writetoFifo (char* buffer) {
    int ffd = open(FIFO_FILE, O_WRONLY);
    if (ffd == -1)
        handle_error("open()");
    write(ffd, buffer, strlen(buffer));
    close(ffd);
}

void dataMngLog(float avgTemp, node* temp) {
    char buffer[BUFF_SIZE];
    if (temp->data.id < 0) { 
        // log nếu nhận được sensor id không hợp lệ
        snprintf(buffer, BUFF_SIZE, "%d. %s Received sensor data with invalid sensor node ID %d\n", sequenceNum, 
                                                                                                   temp->data.time, 
                                                                                                   temp->data.id);
        printf("%s\n", buffer); 
        writetoFifo(buffer); 
        sequenceNum++;
        
    } else {
        if (temp->data.temperature < avgTemp) { 
            // log thông báo nhiệt độ quá lạnh
            snprintf(buffer, BUFF_SIZE, "%d. %s The sensor node with ID %d reports it's too cold (avgtemperature = %.2f)\n",  sequenceNum, 
                                                                                                                            temp->data.time, 
                                                                                                                            temp->data.id,
                                                                                                                            avgTemp);
            printf("%s\n", buffer);
            writetoFifo(buffer);
            sequenceNum++;  
        } else if (temp->data.temperature > avgTemp) { 
            // log thông báo nhiệt độ quá nóng
            snprintf(buffer, BUFF_SIZE, "%d. %s The sensor node with ID %d reports it's too hot (avgtemperature = %.2f)\n",   sequenceNum, 
                                                                                                                            temp->data.time, 
                                                                                                                            temp->data.id,
                                                                                                                            avgTemp);
            printf("%s\n", buffer); 
            writetoFifo(buffer);
            sequenceNum++;
        }
    }
}

void dataMngReadSensorMap() {
    char buffer[BUFF_SIZE];
    int ssmap_fd = open("sensor.map", O_CREAT | O_RDONLY, 0666);
    if (ssmap_fd == -1) {
        handle_error("open()");
    }
    ssize_t byte_read = read(ssmap_fd, buffer, BUFF_SIZE);
    if (byte_read < 0) {
        handle_error("read()");
    }
    
}

void newconnectionLog(int sequenceNum, sensor_data_t recv_data, char* buffer) {
    snprintf(buffer, BUFF_SIZE, "%d. %s A sensor node with ID %d has opened a new connection\n", sequenceNum, 
                                                                                                 recv_data.time, 
                                                                                                 recv_data.id);
    printf("%s\n", buffer);
    writetoFifo(buffer);
}
