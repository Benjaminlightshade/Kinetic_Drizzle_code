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

// State machine state, controlled by the control task
enum SystemState  {CALIBRATING, RUNNING, ERROR};
enum SystemState state;

// Compute position is the ideal position that the drops should be in. 
struct computePositions{
    int16_t positions[x_size][y_size];
    SemaphoreHandle_t computePositionsMutex;
};
struct computePositions computePos;


void controlTask(void *pvparameter) {
    while (1) {
        switch (state) {
            case CALIBRATING:
                // trigger SPI task to move up
                if (calibration_done()) state = RUNNING;
                break;

            case RUNNING:
                // normal operation
                break;

            case ERROR:
                // stop everything
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void computeTask(void *pvparameter){
    while(1){
        
        BaseType_t xResultCompute;
        static uint8_t status = 0;
        int16_t positions[x_size][y_size]; 



        // Calibrate the drops
        // TBC: to be handled inside computenextpositions
        if(status == 0){
            calibratePositions();
            status++; 
        }


        // Compute the next positions for the drops in the pattern
        // TBC: to give positions array to the compute function.
        status = computeNextPositions(positions);

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

void setup(){

    // Startup config
    state = CALIBRATING;
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

    // Initialize the compute positions struct
    for (int x = 0; x < x_size; x++){
        for (int y = 0; y < y_size; y++){
            computePos.positions[x][y] = 0;
        }
    }
    computePos.computePositionsMutex = xSemaphoreCreateMutex();


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
