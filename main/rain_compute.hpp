#ifndef RAIN_COMPUTE_HPP
#define RAIN_COMPUTE_HPP

#include "config.h"
#include <stdint.h>

// ---------------------------------------------------------------------------
// Forward declarations
// ---------------------------------------------------------------------------

enum sequencer_states{
    SEQ1, 
    SEQ2, 
    SEQ3, 
    SEQ4,
    SEQ5,
    SEQ6,
    SEQ7,
    SEQ8,
    SEQ9,
    SEQ_END
};

enum seq_returns{
    SEQ_INCOMPLETE,
    SEQ_COMPLETE
};
// ---------------------------------------------------------------------------
// sequences base class
// ---------------------------------------------------------------------------

#ifdef __cplusplus 

// Sequence patterns. Create the sequences by inheriting this class. 

class sequences {
public:
    virtual ~sequences() = default;

    /* Moves down to the maximum and then back up */
    int test1(int pos[x_size][y_size], uint64_t elapsed_us);
    int pattern2(int pos[x_size][y_size], uint64_t elapsed_us);
};

// ---------------------------------------------------------------------------
// Sequencer class
// ---------------------------------------------------------------------------

class Sequencer : public sequences {
public:

    uint64_t seq_time_start;
    uint64_t seq_time;
    int seq_state;

    using seq_ptrs = int (sequences::*)(int[x_size][y_size], uint64_t);

    seq_ptrs seq_table[SEQ_END];

    Sequencer();

    void advance_seq();
    void select_seq(int new_state);
    void update_seq_time_start();
    void update_seq_time();

    void get_new_positions(int pos[x_size][y_size]);
};

#endif // __cplusplus

#ifdef __cplusplus
// Wrapper functions for C linkage
extern "C" {
#endif

Ret_t initComputeNextPositions();
Ret_t computeNextPositions(int pos[x_size][y_size], SystemState state);

#ifdef __cplusplus
}
#endif

#endif // RAIN_COMPUTE_H