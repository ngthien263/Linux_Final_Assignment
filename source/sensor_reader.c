#include "sensor_reader.h"
#include <string.h>      
#include <sys/socket.h>         
#include "linkedList.h"
#include "common.h"
    
node* head = NULL;
  
void sensor_reader_init(sensor_info_t* self){
    self->id = -1;                    
    self->data.temperature = 0.0f;
    self->data.humidity    = 0.0f;
    memset(self->timestamp, 0, sizeof self->timestamp);
    self->state.connected = false;
    self->state.new_data = false;
}

void sensor_reader_cleanup(sensor_info_t* self){
    free(self);
}

static void __read_fifo(int socket_fd){
    char recvbuff[BUFF_SIZE];
    memset(recvbuff, 0, BUFF_SIZE);
    if (recv(socket_fd, recvbuff, BUFF_SIZE, 0) < 0)
        handle_error("read()");
    char *line = strtok(recvbuff, "\n");
    while (line != NULL) {
        sensor_info_t recv_info;
        sscanf(line, "%s - ID: %dTemp: %f Humid: %f\n", recv_info.timestamp,
                                                        &recv_info.id,
                                                        &recv_info.data.temperature,
                                                        &recv_info.data.humidity);
        printf("%s - ID: %dTemp: %f Humid: %f\n", recv_info.timestamp,
                                                  recv_info.id,
                                                  recv_info.data.temperature,
                                                  recv_info.data.humidity);
        pushBack(&head, recv_info);
        line = strtok(NULL, "\n");
    }
}

sensor_info_t* sensor_reader_create(void){
   sensor_info_t* sensor = (sensor_info_t*)malloc(sizeof(sensor_info_t));
   if(sensor != NULL){
        sensor_reader_init(sensor);
   }
   return sensor;
}


void sensor_read(sensor_info_t** sensors, int socket_fd){
    __read_fifo(socket_fd);
    while(head != NULL) {
        sensor_info_t temp;
        popFront(&head, &temp);
        for(int i = 0; i < MAX_SENSORS; i++){
            if(!sensors[i]){
                sensors[i] = sensor_reader_create();
                sensors[i]->id = temp.id;
                sensors[i]->data = temp.data;
                strcpy(sensors[i]->timestamp, temp.timestamp);
                sensors[i]->state.new_data = false;
                break;
            } else {
                if(sensors[i]->id == temp.id){
                    sensors[i]->data = temp.data;
                    strcpy(sensors[i]->timestamp, temp.timestamp);
                    sensors[i]->state.new_data = false;
                    break;
                }
            }
        }
    }
}
