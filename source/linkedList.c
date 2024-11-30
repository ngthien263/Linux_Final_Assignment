#include "linkedList.h"
#include "log.h"

node* makeNode(sensor_data_t inputData) {
    node* new_node = (node*)malloc(sizeof(node));
    new_node->data = inputData;
    new_node->next = NULL;
    new_node->prev = NULL;
    return new_node;
}

void pushBack(node** fhead, sensor_data_t inputData) {
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
