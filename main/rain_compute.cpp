#include "config.h"
#include "rain_compute.h"
#include "esp_log.h"
#include <memory>

// Global variables //

Sequencer sequencer;

class sequences{
    public: 

        void pattern1(){

        }

        void pattern2(){
        
        }
};

class Sequencer : public sequences{
    public: 

        int new_positions[x_size][y_size];
        static int time_now; 
        static int time_start; 
        int state; 

        Sequencer() {
            // Initialize positions to zero
            for (int x = 0; x < x_size; x++) {
                for (int y = 0; y < y_size; y++) {
                    new_positions[x][y] = 0;
                }
            }

            state = DEFAULT
        }

        void update_state(){
        }
    
        void run(int *pPositions){ {
            // Simple test: increment all positions by 1
            for (int x = 0; x < x_size; x++) {
                for (int y = 0; y < y_size; y++) {
                    new_positions[x][y] += 1;
                }
            }
        }
    }
    
};


// Task Functions //

Ret_t initComputeNextPositions() {

    sequencer = Sequencer();

    sequencer.state = DEFAULT;

    ESP_LOGI("Sequence", "Sequence table initialized");

    return SUCCESS;
}


// Main compute function //
// Computs the next positions based on the current system state //
Ret_t computeNextPositions(int *pPositions, SystemState state){
    
    sequencer.run(int* pPositions);

}


// Log the positions to serial for debugging.
// Continue from here!

