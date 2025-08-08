#include <fcntl.h>      
#include <stdbool.h>
#include <unistd.h>    
#include "common.h"
#include "thread_handle.h"
#include "sensor_reader.h"
extern sensor_info_t* sensors[];

void* connect_thread_handler(void* args){
    char buffer[1024];
    thr_handle_t* handle = (thr_handle_t*)args;
    int socket_fd = socket_connect(&handle->thr_socket);
    int f_fd = open("./log_fifo", O_WRONLY);
    if (f_fd < 0) handle_error("open fifo ");
    while(1){
        sensor_read(sensors, socket_fd);
        pthread_mutex_lock(&handle->mlock);
        for(int i = 0; i < MAX_SENSORS; i++){
            if(sensors[i] && !sensors[i]->state.connected){
                int len = sprintf(buffer, "%d. %s A sensor node with ID %d has opened a new connection\n", handle->sequence_number,
                                                                                                sensors[i]->timestamp,
                                                                                                sensors[i]->id);
                write(f_fd, buffer, len);
                sensors[i]->state.connected = true;
                handle->sequence_number++;
            }
        }   
        pthread_mutex_unlock(&handle->mlock);
    }  
    close(f_fd);
    pthread_exit(NULL);
}


void* data_manager_thread_handle(void* args){
    thr_handle_t* handle = (thr_handle_t*)args;
    char buffer[1024];
    int f_fd = open("./log_fifo", O_WRONLY);
    static float avg_temp = 0.;
    static float avg_humid = 0.;
    static int division = 0;
    while(1){
        pthread_mutex_lock(&handle->mlock);
        for(int i = 0; i < MAX_SENSORS; i++){
            if(sensors[i] && sensors[i]->state.new_data == false && sensors[i]->state.connected == true){
                avg_temp  += sensors[i]->data.temperature;
                avg_humid += sensors[i]->data.humidity;
                division++;
            }
        }
        if(division > 0){
            avg_temp /= division;
            avg_humid /= division;
        }
        pthread_mutex_unlock(&handle->mlock);
        int len = 0;
        for(int i = 0; i < MAX_SENSORS; i++){
            if(sensors[i] && sensors[i]->state.connected == true && sensors[i]->state.new_data == false){
                if(sensors[i]->data.temperature > TEMP_UPPER_LIMIT) {
                    len = sprintf(buffer, "[Data] Sensor %d too hot (avg=%.2f)\n", sensors[i]->id, avg_temp); 
                }         
                else if (sensors[i]->data.temperature < TEMP_LOWER_LIMIT){
                    len = sprintf(buffer, "[Data] Sensor %d too cold (avg=%.2f)\n", sensors[i]->id, avg_temp);    
                } 
                else 
                    len = sprintf(buffer, "[Data] Normal temperature (avg = %.2f)",  avg_temp);   
                write(f_fd, buffer, len);   
                sensors[i]->state.new_data = true; 
            }
        }
    }
    close(f_fd);
    pthread_exit(NULL);
}