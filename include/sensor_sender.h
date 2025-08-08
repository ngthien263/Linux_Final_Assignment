#ifndef SENSOR_SENDER_H
#define SENSOR_SENDER_H
#include "sensor_types.h"
void sensor_send_message(sensor_info_t** sensors, int server_fd);
#endif