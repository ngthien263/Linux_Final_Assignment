#include <string.h>  
#include <unistd.h>  
#include <sys/time.h>
#include "sensor_sender.h"
#include "common.h"
static void get_time(char *buffer) {
    time_t t = time(NULL);
    struct tm* timeinfo = localtime(&t);
    snprintf(buffer, 64, "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}

sensor_info_t sensor_init(int sensor_id){
    sensor_info_t new_sensor = {0};
    new_sensor.id = sensor_id;
    get_time(new_sensor.timestamp);
    return new_sensor;
}

sensor_node_t* sensor_create(int sensor_id){
    sensor_node_t* new_sensor = (sensor_node_t*)malloc(sizeof(sensor_node_t));
    new_sensor->info = sensor_init(sensor_id);
    new_sensor->next = NULL;
    return new_sensor;
}

void add_sensor(sensor_node_t** head_sensor, int sensor_id){
    sensor_node_t* new_sensor = sensor_create(sensor_id);
    if(*head_sensor == NULL){
        *head_sensor = new_sensor;
        return;
    }
    sensor_node_t* temp = *head_sensor;
    while(temp->next != NULL){
        temp = temp->next;
    }
    temp->next = new_sensor;
}

void rm_sensor(sensor_node_t** head_sensor, int sensor_id){
    sensor_node_t* curr = *head_sensor;
    sensor_node_t* prev = NULL;
    while (curr != NULL) {
        if(curr->info.id == sensor_id){
            if(prev == NULL) {
                *head_sensor = curr->next;
            } else
                prev->next = curr->next;
            free(curr);
            printf("Sensor ID %d removed\n", sensor_id);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    printf("Sensor ID %d not found\n", sensor_id);
}

void dump_sensor(sensor_node_t** head_sensor) {
    int count = 1;
    sensor_node_t* temp = *head_sensor;
    while(temp != NULL){
        printf("%d. ID: %d\n", count++, temp->info.id);
        temp = temp->next; 
    }
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
    snprintf(buff,BUFF_SIZE, "%s - ID: %dTemp: %.2f Humid: %.2f\n", sensor_data->timestamp,
                                                         self->id,
                                                         sensor_data->data.temperature,
                                                         sensor_data->data.humidity);
}


void sensor_send_message(sensor_node_t** sensor_head, int server_fd) {
    char buff[BUFF_SIZE];
    sensor_node_t* curr = *sensor_head;
    while(curr != NULL){
        sensor_generate_message(&curr->info, buff);
        if(write(server_fd, buff, strlen(buff)) == -1)
             handle_error("write()");
        curr = curr->next;
    }
}