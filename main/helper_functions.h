#ifndef HELPER_H
#define HELPER_H

#include "config.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_err.h"


// Core functions


// void updateShiftRegister(int dataPin, int latchPin, int clockPin, byte regVal);

// void interruptFunc();

void calibratePositions();


// Helper functions for the main loop

void zeroPositionArrays(int *positions, int *nextPositions, int xsize, int ysize);

void setBit(unsigned char *mychar, int n, int x);

void removeStepBits(unsigned char *byteArr, int size);




#endif // HELPER_H

