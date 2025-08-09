#include <string.h>  
#include <unistd.h>  
#include <sys/time.h>
#include "sensor_sender.h"
#include "common.h"

static void get_time(char *buffer) {
    time_t t = time(NULL);
    struct tm* timeinfo = localtime(&t);
    sprintf(buffer, "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}

sensor_info_t* stimulate_sensor_info(sensor_info_t* self){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand((unsigned int)(tv.tv_sec ^ tv.tv_usec));
    self->data.temperature = rand() % 100 + 1;
    self->data.humidity = rand() % 100 + 1;
    get_time(self->timestamp);
    return self;
}


void sensor_generate_message(sensor_info_t* self, char* buff){
    memset(buff, 0, BUFF_SIZE);
    sensor_info_t* sensor_data = stimulate_sensor_info(self);
    sprintf(buff, "%s - ID: %dTemp: %.2f Humid: %.2f\n", sensor_data->timestamp,
                                                              self->id,
                                                              sensor_data->data.temperature,
                                                              sensor_data->data.humidity);
}

sensor_info_t* sensor_create(void){
   sensor_info_t* sensor = (sensor_info_t*)malloc(sizeof(sensor_info_t));
   if(sensor != NULL){
        sensor->id = rand() % 100 + 1;
        sensor->data.temperature = 0;
        sensor->data.humidity = 0;
        memset(sensor->timestamp, 0, sizeof(sensor->timestamp));
   }
   return sensor;
}

void sensor_send_message(sensor_info_t** sensors, int server_fd) {
    char buff[BUFF_SIZE];
    for(int i = 0; i < MAX_SENSORS; i++){
        if(!sensors[i])
            sensors[i] = sensor_create();
        sensor_generate_message(sensors[i], buff);
        if(write(server_fd, buff, strlen(buff)) == -1)
            handle_error("write()");
        printf("%s", buff);
    }
}

