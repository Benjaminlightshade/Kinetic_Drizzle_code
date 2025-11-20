#ifndef RAIN_COMPUTE_H
#define RAIN_COMPUTE_H

#include "config.h"




// Task functions

Ret_t computeNextPositions(int* pPositions, SystemState state);

enum sequencer_states{
    SEQ_1, 
    SEQ_2
} sequencer_states;


#endif