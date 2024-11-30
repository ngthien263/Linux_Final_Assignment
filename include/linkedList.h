#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include <stdlib.h>
typedef struct {
    int id;
    float temperature;
    char time[30];
} sensor_data_t;

typedef struct node {
    sensor_data_t data;
    struct node* prev;
    struct node* next;
} node;

node* makeNode(sensor_data_t inputData);
void pushBack(node** fhead, sensor_data_t inputData);

#endif // LINKEDLIST_H
