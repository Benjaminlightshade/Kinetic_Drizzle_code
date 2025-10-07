#include <stdio.h>
#include <string.h>

#include "main_tasks.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_timer.h"

#include "helper_functions.h"
#include "rain_types.h"
#include "config.h"

////// Variables /////

// Positions hold the current positions of the drops 
// nextPositions is the target positon of the job in the next cycle

int16_t positions[x_size][y_size]; 
int16_t nextPositions[x_size][y_size]; 
spi_device_handle_t spi_handle = NULL;
esp_err_t ret;


///// Main init functions /////

// Initializes the global arrays to the starting value
void init_main_arrs(){
    for ( int i = 0; i < x_size; i++){
        for ( int j = 0; j < y_size; j++){
            positions[i][j] = start_drop_pos;
            nextPositions[i][j] = start_drop_pos;
        }
    }
}

// Function to init the SPI bus and device
esp_err_t init_spi() {
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
esp_err_t spi_send_data(spi_device_handle_t spi, const unsigned char *data, size_t length) {
  spi_transaction_t t;
  memset(&t, 0, sizeof(t));  // Zero out the transaction
  t.length = length * 8;     // Length is in bits
  t.tx_buffer = data;        // Data to be sent

  //////////// TEST///////////////
  // unsigned char *byte_ptr;
  // for(int i = 0; i < length; i++){
  //   byte_ptr = data + i;
  //   ESP_LOGI("test", "%u in pos %d", *byte_ptr, i);
  // }
  //////////// TEST///////////////

  ret = spi_device_transmit(spi, &t);
  if (ret != ESP_OK) {
      ESP_LOGE("SPI", "Failed to transmit SPI data: %s", esp_err_to_name(ret));
  }

  return ret;
}


// Task functions //

uint8_t computeNextPositions(uint8_t status){
  
  // Run sequence
  // Input for each sequence is time

  // TODO: create the status logic and updates
  
  uint64_t sequenceStartTime;
  uint64_t sequenceTime;

  // Todo : use an array of function points to select which sequence to run.
  // This will reduce the amount of code needed to compute the next positions. 

  // Run the first sequence
  ESP_LOGI("Sequence", "Starting first sequence");
  sequenceStartTime = esp_timer_get_time();
  sequenceTime = sequenceStartTime;
  vTaskDelay(1000/portTICK_PERIOD_MS);

  // Create test sequence
  ESP_LOGI("Sequence", "Test sequence start");
  testMove(sequenceTime, nextPositions);
  ESP_LOGI("Sequence", "Test sequence complete");
  
  // while(SimpleWaveSequence(sequenceTime, *tmpNextPositions, x_size, y_size)){  
  //     sequenceTime = esp_timer_get_time() - sequenceStartTime;
  //     xResultCompute = ulTaskNotifyTakeIndexed(0, pdTRUE, 10/portTICK_PERIOD_MS);
      
  //     if (xResultCompute == 0){
  //         ESP_LOGI("Sequence", "Motor task is not ready. Computing next cycle."); 
  //         continue;
  //     } else {
  //         memcpy(nextPositions, tmpNextPositions, sizeof(nextPositions));
  //         xTaskNotifyGiveIndexed(xMotorTask, 0);
  //     }
  // }

  ESP_LOGI("Sequence", "First sequence complete");

  return status; 

}


// Moves from postions to nextPositions through stepping as needed
void sendSteps(){

  bool incomplete = true;
  int stepBit = 0;
  int dirBit = 0;
  int bitSelect = 0;
  unsigned char *pcbRegs;
  int numberOfBoards = x_size*y_size/2/2;
  unsigned char shiftRegisterOutput[numberOfBoards];

  static int64_t step_time_elapsed = 0;
  
  // Init the memory and start a timer
  // '\0' is 00000000. 
  memset(&shiftRegisterOutput, '\0', sizeof(shiftRegisterOutput));


  // Find the number of steps and direction of steps for each drop 
  // Write the direction and number of steps to the correct binary values in the 
  // shift register output array.
  while(incomplete){
    incomplete = false;

  // To do: The following can be made into functions instead
    for (int x = 0; x < x_size; x++){
      for (int y = 0; y < y_size; y++){
        if (nextPositions[x][y] > positions[x][y]){
          pcbRegs = &shiftRegisterOutput[(numberOfBoards - 1) - ((x/2)+(y/2))];
          stepBit = 1;
          dirBit = 1;
          bitSelect = 7 - ((x%2)*2) - ((y%2)*4);
          setBit(pcbRegs, bitSelect, dirBit);
          setBit(pcbRegs, bitSelect-1, stepBit);
          positions[x][y] = positions[x][y] + 1;
          incomplete = true;
          ESP_LOGI("test", "bit select %d and reg %u", bitSelect, *pcbRegs);

        }else if (nextPositions[x][y] < positions[x][y]){
          pcbRegs = &shiftRegisterOutput[(numberOfBoards - 1) - ((x/2)+(y/2))];
          stepBit = 1;
          dirBit = 0;
          bitSelect = 7 - ((x%2)*2) - ((y%2)*4);
          setBit(pcbRegs, bitSelect, dirBit);
          setBit(pcbRegs, bitSelect-1, stepBit);
          // bitSelect = (x%2)*2 + (y%2)*4;
          // _setBit(pcbRegs, bitSelect, dirBit);
          // _setBit(pcbRegs, bitSelect+1, stepBit);
          positions[x][y] = positions[x][y] -1;
          incomplete = true;
          ESP_LOGI("test2", "bit select %d and reg %u", bitSelect, *pcbRegs);

        }
      }
    }

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

    if (ret != ESP_OK) {
        ESP_LOGE("Main", "Send steps unsuccessful");
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


