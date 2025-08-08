#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include <stdbool.h>
#include <stdlib.h>
#include "sensor_types.h"

typedef struct node {
    sensor_info_t data;
    struct node* prev;
    struct node* next;
} node;

node* makeNode(sensor_info_t inputData);
void pushBack(node** fhead, sensor_info_t inputData);
int popFront(node** fhead, sensor_info_t* outputData);
#endif // LINKEDLIST_H
