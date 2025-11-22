#include "config.h"
#include "rain_compute.hpp"
#include "esp_log.h"
#include "esp_timer.h"
#include <memory>
#include <string.h>

// Global variables //

static Sequencer sequencer;

// Sequences class methods //

int sequences::pattern1(int pos[x_size][y_size], uint64_t elapsed_us){
    int finished = true;

    return false;
}

int sequences::pattern2(int pos[x_size][y_size], uint64_t elapsed_us){
    int finished = true;

    return false;
}

// Sequencer class methods // 

Sequencer::Sequencer()
{
    memset(new_positions, 0, sizeof(new_positions));
    seq_time_start = esp_timer_get_time();
    seq_time = 0;

    seq_state = SEQ1;  // Start at the first sequence

    // Load the function pointer table
    seq_table[SEQ1] = &sequences::pattern1;
    seq_table[SEQ2] = &sequences::pattern2;

}


void Sequencer::advance_state()
{
    seq_state++;

    if (seq_state >= SEQ_END)
        seq_state = SEQ1;
}

void Sequencer::select_state(int new_state)
{
    if (new_state >= SEQ1 && new_state < SEQ_END)
        seq_state = new_state;
}

void Sequencer::update_seq_time_start()
{
    seq_time_start = esp_timer_get_time();
}

void Sequencer::update_seq_time()
{
    uint64_t now = esp_timer_get_time();
    seq_time = now - seq_time_start;
}

void Sequencer::get_new_positions(int pos[x_size][y_size])
{
    update_seq_time();

    // Get pointer to the correct pattern
    seq_ptrs fn = seq_table[seq_state];

    // Run pattern
    int finished = (this->*fn)(pos, seq_time);

    // After finish → move to next pattern and restart timer
    if (finished) {
        advance_state();
        update_seq_time_start();
    }

    // Copy positions to local buffer (optional)
    for (int x = 0; x < x_size; x++)
        for (int y = 0; y < y_size; y++)
            new_positions[x][y] = pos[x][y];
}

extern "C" {


Ret_t initComputeNextPositions()
{

    Ret_t ret = SUCCESS;
    sequencer = Sequencer();

    ESP_LOGI("Sequence", "Sequencer initialized");
    return ret;  // SUCCESS
}

Ret_t computeNextPositions(int pos[x_size][y_size], SystemState state)
{
    Ret_t ret = SUCCESS;

    // The state argument is optional — you may use sequencer internally
    sequencer.get_new_positions(pos);

    return ret; // SUCCESS
}

} // end extern "C"
