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

static int actual_positions[x_size][y_size]; 
spi_device_handle_t spi_handle = NULL;

// Board order for shift register output. 
unsigned int board_order[number_of_boards] = {0,1,2,3,4,5,6,7}; // Order of the boards for shift register output
// Continue from here: the board order needs to be ingrained into actuation movement. 

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
esp_err_t spi_send_data(spi_device_handle_t spi, const uint8_t* data, size_t length) {
  
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

Ret_t compute_to_move(int* target_positions[x_size][y_size]){
  
  Ret_t ret = SUCCESS;
  Ret_t steps_cleared = INCOMPLETE;

  uint8_t shiftRegisterOutput[number_of_boards];


  // Set shift register output to zero
  memset(&shiftRegisterOutput, '\0', sizeof(shiftRegisterOutput));    // '\0' is 00000000. 

  // While loop to send all the steps needed to move to the target positions
  while (steps_cleared == INCOMPLETE){
    // In each loop, each motor can only move 1 step. 
    
    //Get the next steps for all motors, into bits of the shift register output
    steps_cleared = get_steps_to_bytes(target_positions, shiftRegisterOutput, number_of_boards);

    // Check the time since the previous move
    while (move_timer_check() != SUCCESS){
      ESP_LOGI("Actuation", "Waiting for min time to pass");
    }

    // Send the output to the shift registers using SPI
    ret = spi_send_data(spi_handle, shiftRegisterOutput, number_of_boards);

    if (ret == ESP_OK) {
      // Update the new actual positions upon successful SPI transmission
      update_actual_positions(target_positions);
      latch_registers(); 

    } else {
      ESP_LOGE("Actuation", "Failed to send SPI data");
      ret = ERROR;
      return ret;
    }
  }

  return ret; 

}

// Function that takes the target positions and calculates 1 step to take for each motor. 
// Outputs the bytes needed to be sent to SPI to move the motors. 
Ret_t get_steps_to_bytes(int* target_positions[x_size][y_size], uint8_t *byte_arr, unsigned int byte_arr_size){

  Ret_t ret = SUCCESS;
  unsigned int board_index = 0;
  int step = 0;
  int stepBit = 0;
  int dirBit = 0;

  // Calculate the step for each of the motors. 
  for (int x = 0; x < x_size; x++){
    for (int y = 0; y < y_size; y++){

      // Get the step needed to move at that position
      step = target_positions[x][y] - actual_positions[x][y];

      if (step > max_steps_per_cycle){
        // Print a warning
        ESP_LOGW("Actuation", "Large step detected at position (%d, %d): %d steps", x, y, step);
      }

      // Get the index of the byte to be set.  
      board_index = (x /2) + (y /2) * x_size; 
      
      // Get the bits to be changed
      stepBit = 2 * (2 * (x % 2) + (y % 2)) + 1;
      dirBit = 2 * (2 * (x % 2) + (y % 2));

      // Set bits in the shift register output
      if (step != 0){
        set_bits_in_byte_arr(step, stepBit, dirBit, byte_arr, board_index);
        ret = INCOMPLETE; 
      }
    }
  }

  return ret;

}

// Sets the corresponding bits in the byte array for the step and direction
void set_bits_in_byte_arr(int step, int stepBit, int dirBit, uint8_t *byte_arr, unsigned int board_index){

  if (step > 0){
    // Move down
    // Set the step bit to 1
    bitWrite(byte_arr[board_index], stepBit, 1);
    // Set the dir bit to 0
    bitWrite(byte_arr[board_index], dirBit, 0);

  } else if (step < 0){
    // Move up 
    // Set the step bit to 1
    bitWrite(byte_arr[board_index], stepBit, 1);
    // Set the dir bit to 1
    bitWrite(byte_arr[board_index], dirBit, 1);
  }

}

// Update the actual positions array in memory to reflect the positions of the drops after the move. 
void update_actual_positions(int* target_positions[x_size][y_size]){
  // Update the actual positions of the drops

  for (int x = 0; x < x_size; x++){
    for (int y = 0; y < y_size; y++){
      if (target_positions[x][y] > actual_positions[x][y]){
        actual_positions[x][y] += 1;
      } else if (target_positions[x][y] < actual_positions[x][y]){
        actual_positions[x][y] -= 1;
      }
    }
  }

}

// Checks the time since the last movement to prevent moving under minimum move interval.
Ret_t move_timer_check(){
  
  Ret_t ret = SUCCESS;
  static int64_t last_move_time = 0; 

  // Wait until minimum time has passed
  if((esp_timer_get_time() - last_move_time) < min_time_between_moves){
    ESP_LOGW("Actuation", "Tried to move faster than the time between moves");
    ret = ERROR;
  }

  last_move_time = esp_timer_get_time();

  return ret; 

}

// Latches the shift registers to actuate the motors
void latch_registers(){
  gpio_set_level(rclk_pin,1);
  gpio_set_level(rclk_pin,0);
}


// Defunct
Ret_t steps_to_bytes(int* steps[x_size][y_size], uint8_t *byte_arr, unsigned int byte_arr_size){



  // Log the positions of the drops
  char mystr[100] = {'\0'};
  char temp[6] = {'\0'};
  const char tag[10] = "ROW";

  for (int x = 0; x < x_size; x++){
    for (int y = 0; y < y_size; y++){
         
      // Convert each int16_t to two bytes and format as hex
      uint8_t high_byte = (actual_positions[x][y] >> 8) & 0xFF;
      uint8_t low_byte = actual_positions[x][y] & 0xFF;

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




Ret_t calibration(){

	Ret_t status = SUCCESS;



	// TBC: Calibration logic here


	return status; 

}


