#ifndef CONFIG_H
#define CONFIG_H

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

// Parameters //
#define x_size 4
#define y_size 8
#define number_of_boards (x_size * y_size / 4)

#define min_time_between_moves 100              // Minimum time between each stepper motor step in us

// Constants //
#define max_drop_pos 10000                      // Drops will be at the bottom position
#define start_drop_pos 0                        // Drops will start at position 0, when they are at the top
#define min_drop_pos -10000                     // Maximum that the drops can go up to hit calibration

#define max_steps_per_cycle 10                  // Maximum steps a drop can move in one compute cycle. 
    // A warning is provided if this is exceeded.                                        


// Pins //
#define rclk_pin 22                   // RCLK is the latch pin
#define rclk_pin_bitmask (1ULL<<rclk_pin)

#define PIN_NUM_MISO 12
#define PIN_NUM_MOSI 13
#define PIN_NUM_CLK  14

#define spi_device_hostid SPI2_HOST

#define PI 3.1412

// Global vairables // 

/* 
Compute position is the ideal position that the drops should be in. 
Contains a mutex for safe access across tasks.
*/

typedef struct {
    int positions[x_size][y_size];
    SemaphoreHandle_t computePositionsMutex;
} ComputePositions; 

#define DEF_ERROR 0
#define DEF_SUCCESS 1
#define DEF_INCOMPLETE 2

// Return values // 
typedef enum {
    ERROR = DEF_ERROR,
    SUCCESS = DEF_SUCCESS,
    INCOMPLETE = DEF_INCOMPLETE
} Ret_t; 

// State machine state, controlled by the control task
typedef enum{
    CALIBRATE_STATE, 
    RUNNING_STATE, 
    ERROR_STATE
} SystemState;

#endif