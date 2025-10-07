#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/spi_master.h"
#include "esp_timer.h"

#include "config.h"
#include "main_tasks.h"

static TaskHandle_t xMotorTask, xComputeTask = NULL;

void computeTask(void *pvparameter){
    while(1){
        
        BaseType_t xResultCompute;
        static uint8_t status = 0;
        // Calibrate the drops
        if(status == 0){
            calibratePositions();
            status++; 
        }


        // Compute the next positions for the drops in the pattern
        status = computeNextPositions();
        xResultCompute = ulTaskNotifyTakeIndexed(0, pdTRUE, 10/portTICK_PERIOD_MS);

        // Write the next positions only if motor task is not working on it
        if (xResultCompute != 0){
            writeNextPositions();
            xTaskNotifyGiveIndexed(xMotorTask, 0);
        } else {
            ESP_LOGI("Sequence", "Motor task is not ready. Computing next cycle."); 
            continue;
        }

    }
}

void actuatorMotorTask(void *pvparameter){
    while(1){

        // Wait for a task notification from the compute task
        BaseType_t xResultMotor;
        xResultMotor = ulTaskNotifyTakeIndexed(0, pdTRUE, 5000/portTICK_PERIOD_MS);

        // No notification received within the timeout period, go back to waiting. 
        if (xResultMotor == 0){
            ESP_LOGI("Sequence", "Compute task is not ready."); 
            continue;
        }

        // If notification received, notify the compute task and send steps.
        xTaskNotifyGiveIndexed(xComputeTask, 0);
        sendSteps();
    }
}


void app_main(void)
{

    esp_err_t ret;
    // Initalize the SPI connection
    ret = init_spi();
    if (ret != ESP_OK) {
        ESP_LOGE("Main", "Failed to initialize SPI");
        return;
    }
    
    ret = init_gpio();
    if (ret != ESP_OK) {
        ESP_LOGE("Main", "Failed to initialize SPI");
        return;
    }

    init_main_arrs();

    // Create the motor and compute tasks
    xTaskCreate(actuatorMotorTask, "Motor", 2048, NULL, 1, &xMotorTask);
    xTaskCreate(computeTask, "Computing", 2048, NULL, 1, &xComputeTask);

    // Give the first notification to start the compute task rolling
    xTaskNotifyGiveIndexed(xComputeTask, 0);


    #ifdef var
        
    #endif
    // xTaskCreate(testTask, "Test", 2048, NULL, 1, &xTestTask);


    
}
