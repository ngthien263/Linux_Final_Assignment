#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <semaphore.h>

typedef enum {
    CMD_NONE = 0,
    CMD_ADD,
    CMD_REMOVE
} command_type;

typedef struct {
    sem_t sem;               // binary semaphore for synchronization
    int has_command;         // 0: no command, 1: has command
    command_type cmd;
    int sensor_id;
} shared_struct_t;

#endif