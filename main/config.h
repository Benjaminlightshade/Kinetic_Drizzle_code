#ifndef CONFIG_H
#define CONFIG_H

// Parameters //
#define x_size 4
#define y_size 8
#define number_of_boards (x_size * y_size / 4)

#define min_time_between_moves 100              // Minimum time between each stepper motor step in us

// Constants //
#define max_drop_pos 10000                      // Drops will be at the bottom position
#define start_drop_pos 0                        // Drops will start at position 0, when they are at the top
#define zeroed_drop_pos -50                     // Position when the drop is hits the limit switch
#define min_drop_pos -10000                     // Maximum that the drops can go up to hit calibration

#define max_steps_per_cycle 10                  // Maximum steps a drop can move in one compute cycle. 
    // A warning is provided if this is exceeded.                                        


// Pins //
#define PIN_NUM_RCLK 22 // Shift register RCLK - latch pin
#define rclk_pin_bitmask (1ULL<<PIN_NUM_RCLK)

#define PIN_NUM_MISO 12 // Shift register MISO 
#define PIN_NUM_MOSI 13 // Shift register MOSI
#define PIN_NUM_CLK  14 // Shift register CLK

#define spi_device_hostid SPI2_HOST

#define PIN_NUM_LIMIT_SWITCH 0 // Limit switch pin/
#define limit_switch_pin_bitmask (1ULL<<PIN_NUM_LIMIT_SWITCH)

#define PI 3.1412

// Global constants // 

#define DEF_ERROR 0
#define DEF_SUCCESS 1
#define DEF_INCOMPLETE 2

#define GPIO_LIMIT_TRIGGERED 1
#define GPIO_LIMIT_NOT_TRIGGERED 0

// Return values // 
typedef enum {
    ERROR = DEF_ERROR,
    SUCCESS = DEF_SUCCESS,
    INCOMPLETE = DEF_INCOMPLETE
} Ret_t; 

#endif