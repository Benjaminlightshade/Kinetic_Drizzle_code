#include <stdio.h>
#include <string.h>

#include "actuation.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_timer.h"
#include "esp_log.h"

#include "helper_functions.h"
#include "rain_compute.h"
#include "config.h"

////// Variables /////

// Positions hold the current positions of the drops 

static int positions[x_size][y_size]; 
spi_device_handle_t spi_handle = NULL;

////// Utility functions /////

// Function to init the SPI bus and device
esp_err_t init_spi() {
  esp_err_t ret = ESP_OK;
  spi_bus_config_t buscfg = {
      .mosi_io_num = PIN_NUM_MOSI,
      .miso_io_num = -1,
      .sclk_io_num = PIN_NUM_CLK,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = 4096,
  };

  ret = spi_bus_initialize(spi_device_hostid, &buscfg, 1);
  if (ret != ESP_OK) {
    ESP_LOGI("SPI", "Failed to initialize SPI bus: %s", esp_err_to_name(ret));
    return ret;
  }

  spi_device_interface_config_t devcfg = {
      .mode = 0,                          // SPI mode 0
      .clock_speed_hz = 10 * 1000 * 1000, // 10 MHz
      .spics_io_num = -1,         // CS pin
      .queue_size = 8,                    // Queue up to 8 transactions at a time
  };

  ret = spi_bus_add_device(spi_device_hostid, &devcfg, &spi_handle);
  if (ret != ESP_OK) {
    ESP_LOGE("SPI", "Failed to add SPI device: %s", esp_err_to_name(ret));
    spi_bus_free(SPI1_HOST);
    return ret;
  }



  return ESP_OK;
}

// Function to init the gpio pins that will latch the shift registers
esp_err_t init_gpio(){
  esp_err_t ret = ESP_OK;
  gpio_config_t io_conf = {};

  io_conf.pin_bit_mask = rclk_pin_bitmask;
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pull_down_en = 1;
  ret = gpio_config(&io_conf);

  // ret = gpio_set_direction(latch_pin, GPIO_MODE_OUTPUT);
  if (ret != ESP_OK) {
    ESP_LOGI("SPI", "Failed to initialize SPI bus: %s", esp_err_to_name(ret));
    return ret;
  }
  return ret;
}

// Function to send data using SPI to the shift registers
esp_err_t spi_send_data(spi_device_handle_t spi, const unsigned char* data, size_t length) {
  
  spi_transaction_t t;
  esp_err_t ret;

  memset(&t, 0, sizeof(t));  // Zero out the transaction
  t.length = length * 8;     // Length is in bits
  t.tx_buffer = data;        // Data to be sent

  ret = spi_device_transmit(spi, &t);
  if (ret != ESP_OK) {
      ESP_LOGE("SPI", "Failed to transmit SPI data: %s", esp_err_to_name(ret));
  }

  return ret;
}


////// Task functions ///////

Ret_t compute_to_move(int* target_positions[x_size][y_size], int* steps[x_size][y_size]){
  
  Ret_t ret = SUCCESS;
  int steps[x_size][y_size] = {0};

  calculate_move(target_positions, steps);

  send_steps(steps); 

  return ret; 

}

void calculate_move(int* target_positions[x_size][y_size], int* steps[x_size][y_size]){

  // Calculate the steps needed for each drop to move to the next position

  for (int x = 0; x < x_size; x++){
    for (int y = 0; y < y_size; y++){
      steps[x][y] = target_positions[x][y] - positions[x][y];
    }
  }
}

// Moves from postions to nextPositions through stepping as needed
Ret_t send_steps(int* steps[x_size][y_size]){

  // Send the steps to the shift registers to move the motors accordingly
  Ret_t ret = SUCCESS;
  unsigned char shiftRegisterOutput[number_of_boards];

  memset(&shiftRegisterOutput, '\0', sizeof(shiftRegisterOutput));    // '\0' is 00000000. 

  int stepBit = 0;
  int dirBit = 0;
  int bitSelect = 0;
  unsigned char *pcbRegs;
  int64_t step_time_elapsed = 0;

  bool incomplete = true;

  while (incomplete) {
    incomplete = false;

    // Write the bits into the collection(s) of bytes
    for (int x = 0; x < x_size; x++) {
      for (int y = 0; y < y_size; y++) {

        // Formula for selecting the correct step and dir bit in the collection
        stepBit = 2 * (2 * (x % 2) + (y % 2)) + 1;
        dirBit = 2 * (2 * (x % 2) + (y % 2));

        if(steps[x][y] > 0){
          steps[x][y] = steps[x][y] - 1;
          incomplete = true;
          bitWrite(shiftRegisterOutput[(number_of_boards - 1) - ((x/2)+(y/2))], stepBit, 1);
          bitWrite(shiftRegisterOutput[(number_of_boards - 1) - ((x/2)+(y/2))], dirBit, 0);
        }
        
        if(steps[x][y] < 0){
          steps[x][y] = steps[x][y] + 1;
          incomplete = true;
          bitWrite(shiftRegisterOutput[(number_of_boards - 1) - ((x/2)+(y/2))], stepBit, 1);
          bitWrite(shiftRegisterOutput[(number_of_boards - 1) - ((x/2)+(y/2))], dirBit, 1);
        }

      }
    }



  } 


  

/*
  for (int x = 0; x < x_size; x++){
    for (int y = 0; y < y_size; y++){
      if (new_positions[x][y] > positions[x][y]){
        pcbRegs = &shiftRegisterOutput[(numberOfBoards - 1) - ((x/2)+(y/2))];
        stepBit = 1;
        dirBit = 1;
        bitSelect = 7 - ((x%2)*2) - ((y%2)*4);
        setBit(pcbRegs, bitSelect, dirBit);
        setBit(pcbRegs, bitSelect-1, stepBit);
        positions[x][y] = positions[x][y] + 1;
        incomplete = true;
        ESP_LOGI("test", "bit select %d and reg %u", bitSelect, *pcbRegs);

      }else if (new_positions[x][y] < positions[x][y]){
        pcbRegs = &shiftRegisterOutput[(numberOfBoards - 1) - ((x/2)+(y/2))];
        stepBit = 1;
        dirBit = 0;
        bitSelect = 7 - ((x%2)*2) - ((y%2)*4);
        setBit(pcbRegs, bitSelect, dirBit);
        setBit(pcbRegs, bitSelect-1, stepBit);
        positions[x][y] = positions[x][y] -1;
        incomplete = true;
        ESP_LOGI("test2", "bit select %d and reg %u", bitSelect, *pcbRegs);

      }
    }
  }
*/
  while(incomplete){
    incomplete = false;
    
    // Send the output to the shift registers using SPI
    if(incomplete == true){
      ret = spi_send_data(spi_handle, shiftRegisterOutput, sizeof(shiftRegisterOutput));
    
      // Check if sufficient time has elapsed before latching the data and moving the motors
      if(esp_timer_get_time() - step_time_elapsed > min_time_between_moves){
        // Latch to output the shift register data to step the motors
        gpio_set_level(rclk_pin,1);
        gpio_set_level(rclk_pin,0);

        // Log the time at this latch
        step_time_elapsed = esp_timer_get_time();
      }
    }

  }

  // Log the positions of the drops
  char mystr[100] = {'\0'};
  char temp[6] = {'\0'};
  const char tag[10] = "ROW";

  for (int x = 0; x < x_size; x++){
    for (int y = 0; y < y_size; y++){
         
      // Convert each int16_t to two bytes and format as hex
      uint8_t high_byte = (positions[x][y] >> 8) & 0xFF;
      uint8_t low_byte = positions[x][y] & 0xFF;

      // Format the bytes into a hex string and append to the buffer
      snprintf(temp, sizeof(temp), "%02X%02X ", high_byte, low_byte);
      strncat(mystr, temp, 6);  
    }    

    // Sends the messages in x rows
    char new_tag[30] = {'\0'};
    sprintf(new_tag, "%s%d",tag, x);
    ESP_LOGI(new_tag, "%s", mystr);
    memset(new_tag, '\0', 30);
    memset(mystr, '\0', sizeof(mystr));
  }    

}

// Embeds the step in the bits of the corresponding shift register bytes
Ret_t set_shift_registers(){

}

Ret_t calibration(){

	Ret_t status = SUCCESS;



	// TBC: Calibration logic here


	return status; 

}


