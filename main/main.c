#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "driver/uart.h"

#include "string.h"

#include "config.h"
#include "actuation.h"
#include "rain_compute.hpp"

// Global variables //
TaskHandle_t xMotorTask, xComputeTask = NULL;
SystemState sys_state;
ComputePositions computePos;    

// TBC : control task transitions and uses
void controlTask(void *pvparameter) {
    while (1) {

        // Receive byte from UART
        const uart_port_t uart_num = UART_NUM_0;
        uint8_t data[128];
        int length = 0;
        ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num, (size_t*)&length));
        length = uart_read_bytes(uart_num, data, length, 100);

        if(length > 0 ){
            ESP_LOGI("ControlTask", "Received %d bytes from UART", length);
            ESP_LOGI("ControlTask", "Data: %.*s", length, data);

            

        }


        switch (sys_state) {
            case CALIBRATE_SYS_STATE:
                
                // trigger SPI task to move up
                break;

            case RUNNING_SYS_STATE:
                // normal operation
                break;

            case ERROR_SYS_STATE:
                // stop everything
                break;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void computeTask(void *pvparameter){
    while(1){
        
        // Calibration is handled by the actuation task directly
        if (sys_state != RUNNING_SYS_STATE){
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }


        // Temporary positions array used for computation
        int positions[x_size][y_size]; 
        memset(positions, 0, sizeof(positions));

        // Compute the next positions for the drops in the pattern
        if (computeNextPositions(positions) != SUCCESS){
            // Handle error
            ESP_LOGE("ComputeTask", "Failed to compute next positions");
            continue;
        }

        // Write the new positions to the shared ComputePositions struct after computation
        if(xSemaphoreTake(computePos.computePositionsMutex, pdMS_TO_TICKS(100)) == pdTRUE){
            
            for(int x = 0; x < x_size; x++){
                for(int y = 0; y < y_size; y++){
                    computePos.positions[x][y] = positions[x][y];
                }
            }

            // Release the mutex
            xSemaphoreGive(computePos.computePositionsMutex);
            
            // // Notify the motor task that new positions are ready
            // xTaskNotifyGiveIndexed(xMotorTask, 0);

        } else {
            ESP_LOGE("ComputeTask", "Failed to take computePositionsMutex");
            continue;
        }
    }
}

void actuatorMotorTask(void *pvparameter){
    while(1){

        Ret_t status = SUCCESS;

        if (sys_state == CALIBRATE_SYS_STATE){
            status = calibration();

            if (status == SUCCESS){
                ESP_LOGI("ActuationTask", "Calibration successful. Entering running state");
                sys_state = RUNNING_SYS_STATE;
            } else {
                ESP_LOGE("ActuationTask", "Calibration failed. Entering error state");
                sys_state = ERROR_SYS_STATE;
            }

        }

        if (sys_state == ERROR_SYS_STATE){
            // In error state, do not actuate
            ESP_LOGI("ActuationTask", "System in error state. Actuation task halted");
            vTaskDelay(pdMS_TO_TICKS(100));
            continue; 
        }

        
        // Take the mutex to access the computed positions
        if(sys_state == RUNNING_SYS_STATE){

            if (xSemaphoreTake(computePos.computePositionsMutex, 100/portTICK_PERIOD_MS) == pdTRUE){
            
                // Run the actuation of the motors for the given positions
                status = compute_to_move(computePos.positions);
                xSemaphoreGive(computePos.computePositionsMutex);

                if (status != SUCCESS){
                    ESP_LOGE("ActuationTask", "Issue detected.");
                    sys_state = ERROR_SYS_STATE;
                    continue;
                }

            }
        }
    }
}

void setup(){

    // Startup config
    esp_err_t ret;

    // Initalize the SPI connection
    ret = init_spi();
    if (ret != ESP_OK) {
        ESP_LOGE("Main", "Failed to initialize SPI");
        return;
    }
    
    // Initialize the GPIO pins
    ret = init_gpio_shift_latch();
    if (ret != ESP_OK) {
        ESP_LOGE("Main", "Failed to initialize GPIO");
        return;
    }

    ret = init_gpio_limit_switch();
    if (ret != ESP_OK) {
        ESP_LOGE("Main", "Failed to initialize GPIO");
        return;
    }

    // Initialize the compute positions and its mutex
    for (int x = 0; x < x_size; x++){
        for (int y = 0; y < y_size; y++){
            computePos.positions[x][y] = start_drop_pos;
        }
    }
    computePos.computePositionsMutex = xSemaphoreCreateMutex();
    
    initComputeNextPositions(); 

    // Initialize the system state to calibrate 
    sys_state = CALIBRATE_SYS_STATE;

}

void test_print(){
    ESP_LOGI("Test", "Test function executed");
    printf("Hello\n");
}


void app_main(void)
{
    // Initialize variables and peripherals
    setup();
    for (int i = 0; i < 5; i++){
        test_print();
    }
    // Create the motor and compute tasks
    // xTaskCreate(actuatorMotorTask, "Motor", 2048, NULL, 3, &xMotorTask);
    // xTaskCreate(computeTask, "Computing", 2048, NULL, 2, &xComputeTask);
    xTaskCreate(controlTask, "Control", 2048, NULL, 1, NULL);
    
}
