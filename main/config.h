#ifndef CONFIG_H
#define CONFIG_H

#include "driver/gpio.h"

// Parameters //
#define x_size 4
#define y_size 8

#define min_time_between_moves 100              // Minimum time between each stepper motor step in us

// Constants //
#define max_drop_pos 10000
#define start_drop_pos 0
#define min_drop_pos -10000

// Pins //
#define rclk_pin 22                   // RCLK is the latch pin
#define rclk_pin_bitmask (1ULL<<rclk_pin)

#define PIN_NUM_MISO 12
#define PIN_NUM_MOSI 13
#define PIN_NUM_CLK  14

#define spi_device_hostid SPI2_HOST

typedef enum {
    SUCCESS = 0, 
    ERROR = 1
} Ret_t; 

#endif