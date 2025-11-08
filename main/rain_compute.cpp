#include "config.h"
#include "rain_compute.h"
#include "esp_log.h"
#include <memory>


class Sequence {
    public:
        virtual void run(uint64_t time, int iPositions[x_size][y_size]) = 0;
};

class TestSequence : public Sequence {
    public:
        void run(uint64_t time, int iPositions[x_size][y_size]) override {
            // Simple test: increment all positions by 1
            for (int x = 0; x < x_size; x++) {
                for (int y = 0; y < y_size; y++) {
                    iPositions[x][y] += 1;
                }
            }
        }
};

// Task Functions //
static Sequence* stateToSequence[3];

Ret_t initComputeNextPositions() {

    stateToSequence[CALIBRATE_STATE] = make_unique<calibrationSequence>();

    ESP_LOGI("Sequence", "Sequence table initialized");
    return SUCCESS;
}

Ret_t computeNextPositions(int *pPositions, SystemState state){
       
    if (state == CALIBRATE_STATE){
        // If state is calibrate, run calibration sequence


    } else if (state == RUNNING_STATE){
        // If state is running, run normal sequence


    } else if (state == ERROR_STATE){
        // If state is error, handle error
    }

    // If state is normal, run normal sequence



}



