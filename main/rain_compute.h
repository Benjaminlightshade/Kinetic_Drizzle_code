#ifndef RAIN_COMPUTE_H
#define RAIN_COMPUTE_H

#include "config.h"
#include "stdint.h"

// Task functions

Ret_t computeNextPositions(int iPositions[x_size][y_size]);

// Types of rain sequence

bool testMove(int64_t time, int positions[x_size][y_size]);

// bool SimpleWaveSequence(double time, int *positions, int x, int y);

// bool RandomDropSequence(double time, int *positions, int xSize, int ySize, int *dropOrder);

// bool ResetDrops(int *positions, int xSize, int ySize);

// void shuffleArray(int* array, int size);

#endif