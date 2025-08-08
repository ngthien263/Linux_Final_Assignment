#ifndef SENSOR_READER_H
#define SENSOR_READER_H
#include "sensor_types.h"

void sensor_reader_init(sensor_info_t* self);
void sensor_reader_cleanup(sensor_info_t* self);
sensor_info_t* sensor_reader_create(void);
void sensor_read(sensor_info_t** sensors, int socket_fd);
void reader_loop(int sock_fd, int fifo_fd);
#endif