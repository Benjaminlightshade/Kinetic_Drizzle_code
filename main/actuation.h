#ifndef ACTUATION_H
#define ACTUATION_H

#include "esp_log.h"
#include "esp_err.h"
#include "config.h"

#include "driver/spi_master.h"


// Init functions

esp_err_t init_spi();

esp_err_t init_gpio();

// Utility functions

esp_err_t spi_send_data(spi_device_handle_t spi, const uint8_t* data, size_t length);
void bitWrite(uint8_t *byte_value, int bit_pos, int value);

// Task functions

Ret_t compute_to_move(int target_positions[x_size][y_size]);

Ret_t get_steps_to_bytes(int target_positions[x_size][y_size], uint8_t *byte_arr, unsigned int byte_arr_size);
void set_bits_in_byte_arr(int step, int stepBit, int dirBit, uint8_t *byte_arr, unsigned int board_index);
void update_actual_positions(int target_positions[x_size][y_size]);
Ret_t move_timer_check();
void latch_registers();

Ret_t calibration();


#endif