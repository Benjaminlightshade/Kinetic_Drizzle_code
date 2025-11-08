#ifndef CONFIG_H
#define CONFIG_H

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

// Parameters //
#define x_size 4
#define y_size 8

#define min_time_between_moves 100              // Minimum time between each stepper motor step in us

// Constants //
#define max_drop_pos 10000
#define start_drop_pos 0                        // Drops will start at position 0, when they are at the top
#define min_drop_pos -10000

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


// Return values // 
typedef enum {
    ERROR = 0, 
    SUCCESS = 1
} Ret_t; 

// State machine state, controlled by the control task
typedef enum{
    CALIBRATE_STATE, 
    RUNNING_STATE, 
    ERROR_STATE
} SystemState;

#endif