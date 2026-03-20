#ifndef MAIN_H
#define MAIN_H

#include "config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

/* 
Compute position is the ideal position that the drops should be in. 
Contains a mutex for safe access across tasks.
*/

typedef struct {
    int positions[x_size][y_size];
    SemaphoreHandle_t computePositionsMutex;
} ComputePositions; 

// State machine state, controlled by the control task
typedef enum{
    CALIBRATE_SYS_STATE, 
    RUNNING_SYS_STATE, 
    ERROR_SYS_STATE
} SystemState;

#endif