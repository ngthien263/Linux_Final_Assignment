#ifndef SENSOR_TYPES_H
#define SENSOR_TYPES_H

#include <time.h>
#include <stdio.h>      
#include <stdlib.h>
#define BUFF_SIZE 256
#define MAX_SENSORS 10
#define TEMP_UPPER_LIMIT 50
#define TEMP_LOWER_LIMIT 20

typedef struct {
    float temperature;
    float humidity;
} sensor_data_t;

typedef struct {
    int in_use;
    int connected;
} sensor_state_t;

typedef struct {
    int id;
    sensor_data_t data;
    char timestamp[64];
    sensor_state_t state;
} sensor_info_t;

#endif 