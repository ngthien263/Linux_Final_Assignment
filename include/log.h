#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdlib.h>
#include "linkedList.h"
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define BUFF_SIZE 256
#define FIFO_FILE  "./logfifo"
#define LOG_FILE   "gateway.log"
int sequenceNum;
void writetoFifo(char* buffer);
void dataMngLog(float avgTemp, node* temp);
void newconnectionLog(int sequenceNum, sensor_data_t recv_data, char* buffer);
#endif