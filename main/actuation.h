#ifndef ACTUATION_H
#define ACTUATION_H

#include "esp_log.h"
#include "esp_err.h"

#include "helper_functions.h"
#include "config.h"

// Init functions
void init_main_arrs();

esp_err_t init_spi();

esp_err_t init_gpio();

// Task functions


void writeNextPositions();

void sendSteps();

// Core functions
esp_err_t spi_send_data(spi_device_handle_t spi, const unsigned char *data, size_t length);





#endif