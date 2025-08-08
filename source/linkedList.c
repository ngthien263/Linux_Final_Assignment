#include "linkedList.h"

node* makeNode(sensor_info_t inputData) {
    node* new_node = (node*)malloc(sizeof(node));
    new_node->data = inputData;
    new_node->next = NULL;
    new_node->prev = NULL;
    return new_node;
}

void pushBack(node** fhead, sensor_info_t inputData) {
    node* new_node = makeNode(inputData);
    if (*fhead == NULL) {
        *fhead = new_node;
        return;
    }
    node* temp = *fhead;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = new_node;
    new_node->prev = temp;
}

int popFront(node** fhead, sensor_info_t* outputData) {
    if (fhead == NULL || *fhead == NULL) return false;
    node* temp = *fhead;
    *outputData = temp->data;  
    *fhead = temp->next;     
    if (*fhead != NULL) {
        (*fhead)->prev = NULL;
    }

    free(temp);
    return true;
}