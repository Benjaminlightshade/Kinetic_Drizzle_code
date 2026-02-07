#include "config.h"
#include "rain_compute.hpp"
#include <memory>
#include <string.h>
#include <algorithm>

#ifdef PLATFORM_ESP32
#include "esp_timer.h"
#include "esp_log.h"
#endif

// Global variables //

static Sequencer sequencer;

// Constants //
#define SEQ_TIME_LIMIT 100000000 // 100 seconds per sequence

// Sequences class methods //

int sequences::test1(int pos[x_size][y_size], int64_t elapsed_us){

    int status = SEQ_INCOMPLETE;
    int64_t half_time = SEQ_TIME_LIMIT / 2; 
    int gradient = max_drop_pos / half_time; 
    int calc_pos; 


    if (elapsed_us <= half_time){
        calc_pos = gradient * elapsed_us; 
    } else if (elapsed_us <= SEQ_TIME_LIMIT ){
        calc_pos = max_drop_pos - (gradient * (elapsed_us - half_time));
    } else if (elapsed_us > SEQ_TIME_LIMIT){
        calc_pos = min_drop_pos; 
        status = SEQ_COMPLETE;
    }

    std::fill(&pos[0][0], &pos[0][0] + (x_size * y_size), calc_pos);
    
    return status; 
}

int sequences::pattern2(int pos[x_size][y_size], int64_t elapsed_us){
    int finished = true;

    return false;
}

// Sequencer class methods // 

Sequencer::Sequencer()
{
    seq_time_start = get_time_us();
    seq_time = 0;

    seq_state = SEQ1;  // Start at the first sequence

    // Initialize the function pointer table
    seq_table[SEQ1] = &sequences::test1;
    seq_table[SEQ2] = &sequences::pattern2;

}


void Sequencer::advance_seq()
{
    seq_state++;

    if (seq_state >= SEQ_END)
        seq_state = SEQ1;
}

void Sequencer::select_seq(int new_state)
{
    if (new_state >= SEQ1 && new_state < SEQ_END)
        seq_state = new_state;
}

void Sequencer::update_seq_time_start()
{
    seq_time_start = get_time_us();
}

void Sequencer::update_seq_time()
{
    int64_t now = get_time_us();
    seq_time = now - seq_time_start;
}

void Sequencer::get_new_positions(int pos[x_size][y_size])
{
    update_seq_time();

    // Get pointer to the correct pattern
    seq_ptrs fn = seq_table[seq_state];

    // Run pattern
    int seq_ret = (this->*fn)(pos, seq_time);

    // After finish → move to next pattern and restart timer
    if (seq_ret == SEQ_COMPLETE) {
        advance_seq();
        update_seq_time_start();
    }

}


// Timing functions selected by how the unit is compiled
#ifdef PLATFORM_ESP32

// Timing functions on ESP platform
int64_t get_time_us(){
    
    return esp_timer_get_time();

}

void log_message(const char* tag, const char* message){
    ESP_LOGI(tag, "%s", message);
}



// Logging functions

#elif PC_PLATFORM

// Timing functions when compiled on the PC
int64_t get_time_us(){
    
}

// Logging functions

void log_message(const char* tag, const char* message){
    
}



#endif




// C linkage wrapper functions //
// Used to interface main C code with the C++ sequencer class //

extern "C" {

Ret_t initComputeNextPositions()
{

    Ret_t ret = SUCCESS;
    sequencer = Sequencer();

    log_message("Sequence", "Sequencer initialized");
    return ret;  // SUCCESS
}

Ret_t computeNextPositions(int pos[x_size][y_size])
{
    Ret_t ret = SUCCESS;

    // The state argument is optional — you may use sequencer internally
    sequencer.get_new_positions(pos);

    return ret; // SUCCESS
}

} // end extern "C"
