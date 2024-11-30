#ifndef SENSOR_H
#define SENSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "linkedList.h"
#include "database.h"
#include "socket.h"
#include "log.h"
#define LISTEN_BACKLOG 5
#define BUFF_SIZE 256

sensor_data_t readSensorData(int new_socket_fd);
void sendSensorData(int server_fd);
void simulate_sensor(sensor_data_t* sensor_data, int id);
void get_time(char *buffer);

#endif