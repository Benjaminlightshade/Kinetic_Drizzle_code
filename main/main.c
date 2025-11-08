#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"

#include "string.h"

#include "config.h"
#include "actuation.h"
#include "rain_compute.h"

// Global variables //
TaskHandle_t xMotorTask, xComputeTask = NULL;
SystemState sys_state;
ComputePositions computePos;    

// TBC : control task transitions and uses
void controlTask(void *pvparameter) {
    while (1) {
        switch (sys_state) {
            case CALIBRATE_STATE:
                // trigger SPI task to move up
                break;

            case RUNNING_STATE:
                // normal operation
                break;

            case ERROR_STATE:
                // stop everything
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void computeTask(void *pvparameter){
    while(1){
        
        // Calibration is handled by the actuation task directly
        if (sys_state == CALIBRATE_STATE){
            continue;
        }


        // Temporary positions array used for computation
        int positions[x_size][y_size]; 
        memset(positions, 0, sizeof(positions));

        // Compute the next positions for the drops in the pattern
        if (computeNextPositions((int *)positions, sys_state) != SUCCESS){
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

        if (sys_state == CALIBRATE_STATE){
            calibration();
            continue;
        }

        // // Wait for notification from compute task 
        // if (ulTaskNotifyTakeIndexed(0, pdTRUE, 5000/portTICK_PERIOD_MS) == pdFALSE){
        //     ESP_LOGI("Sequence", "No notification received from compute task."); 
        //     continue;
        // }
        
        // Take the mutex to access the computed positions
        if(xSemaphoreTake(computePos.computePositionsMutex, 100/portTICK_PERIOD_MS) == pdTRUE){
            
            // Run the actuation of the motors for the given positions
            compute_to_move(&computePos.positions);
            xSemaphoreGive(computePos.computePositionsMutex);

        };

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
    ret = init_gpio();
    if (ret != ESP_OK) {
        ESP_LOGE("Main", "Failed to initialize SPI");
        return;
    }

    // Initialize the compute positions and its mutex
    for (int x = 0; x < x_size; x++){
        for (int y = 0; y < y_size; y++){
            computePos.positions[x][y] = start_drop_pos;
        }
    }
    computePos.computePositionsMutex = xSemaphoreCreateMutex();

    // Initialize the system state to calibrate 
    sys_state = CALIBRATE_STATE;

}

void app_main(void)
{
    // Initialize variables and peripherals
    setup();

    // Create the motor and compute tasks
    xTaskCreate(actuatorMotorTask, "Motor", 2048, NULL, 3, &xMotorTask);
    xTaskCreate(computeTask, "Computing", 2048, NULL, 2, &xComputeTask);
    xTaskCreate(controlTask, "Control", 2048, NULL, 1, NULL);

    
}
