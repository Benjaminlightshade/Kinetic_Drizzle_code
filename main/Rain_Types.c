#include "rain_types.h"
#include "helper_functions.h"
#include "config.h"

#define PI 3.1412

// Drop starts at amplitude = 0  when it is on top. max amplitude is provided in amplitude parameter

bool testMove(int64_t time, int16_t nextPositions[x_size][y_size]){

	bool sequenceComplete = false;
	
	nextPositions[0][0] = nextPositions[0][0] + 1;
	nextPositions[1][0] = nextPositions[1][0] + 1;
	nextPositions[0][1] = nextPositions[0][1] + 1;
	nextPositions[1][1] = nextPositions[1][1] + 1;
	sequenceComplete = true; 

	return true;
    
}
/*
// Sine wave from left to right
bool SimpleWaveSequence(double time, int* positions, int xSize, int ySize) {
	double period_ms = 100000;       // Period in ms
	int periods = 5;  // number of oscilations in the sequence
	// int amplitude = 8000;
	int interval = 2000;  // Time between the start/end propogation of each drop
	bool sequenceIncomplete = true;

	// Generate new positions
	// Return the time in the sequence for the next loop

	for (int x = 0; x < xSize; x++) {
		for (int y = 0; y < ySize; y++) {
			// Propogate the pattern based on interval timing
			if (time > (interval * y)) {
				positions[x * ySize + y] = -0.5 * amplitude * (cos((time - (y * interval)) / period_ms * 2 * PI) - 1);
			}
			// Propogate the end of the pattern, returning to setPoint
			if (time > (interval * y + periods * period_ms)) {
				positions[x * ySize + y] = 0;
				// End the sequence if the last 2 points are back at the setPoint
				if (positions[(xSize - 1) * (ySize - 1)] == positions[(xSize - 1) * (ySize - 2)]
					&& positions[(xSize - 1) * (ySize - 1)] == 0) {
					sequenceIncomplete = false;
				}
			}
		}
	}

	return sequenceIncomplete;

}

// Bring drops to the bottom in a random order
bool RandomDropSequence(double time, int *positions, int xSize, int ySize, int *dropOrder){
	// int amplitude = 500;
	int interval = 1000;  // Time between start of each drop in ms 
	int dropTime = 3000;  // Time to reach the bottom in ms
	bool sequenceIncomplete = true;

	for(int counter = 0; counter < xSize * ySize; counter++){
		
		if(time >= (counter*interval) && time <= (counter*interval+dropTime)){
			positions[dropOrder[counter]] = amplitude * time / dropTime;
		}
	}

	if(positions[dropOrder[xSize*ySize-1]] >= amplitude){
		sequenceIncomplete = false;
	}

	return sequenceIncomplete;
}

// Sine waves in different phases, changing with time
bool PhaseChangeWaveSeq(double time, int *positions, int xSize, int ySize){

	double period_ms = 100000;       // Period in ms
	int periods = 5;  // number of oscilations in the sequence
	// int amplitude = 8000;
	int interval = 2000;  // Time between the start/end propogation of each drop
	double phaseDiff;
	bool sequenceIncomplete = true;

	// Generate new positions
	// Return the time in the sequence for the next loop

	for (int x = 0; x < xSize; x++) {
		for (int y = 0; y < ySize; y++) {
			
			// Propogate the pattern based on interval timing
			if (time > (interval * y)) {
				phaseDiff = (time / (period_ms * periods)) * x * PI * 2;
				positions[x * ySize + y] = -0.5 * amplitude * (cos((time - (y * interval)) / period_ms * 2 * PI + phaseDiff) - 1);
			}
/////////////////////////////////////////////////////////////////////////////////////HERE
			// Propogate the end of the pattern, returning to setPoint
			if (time > (interval * y + periods * period_ms)) {
				positions[x * ySize + y] = 0;
				// End the sequence if the last 2 points are back at the setPoint
				if (positions[(xSize - 1) * (ySize - 1)] == positions[(xSize - 1) * (ySize - 2)]
					&& positions[(xSize - 1) * (ySize - 1)] == 0) {
					sequenceIncomplete = false;
				}
			}
		}
	}
	
	return sequenceIncomplete;
}

// Drops randomly move to random target positions
bool RandPosSeq(double time, int *positions){
	bool sequenceIncomplete;
	
	
	return sequenceIncomplete;
}

// Simple triangular wave 
bool LinearWaveSeq(double time, int *positions){
	bool sequenceIncomplete;
	
	
	return sequenceIncomplete;
}

// Triangular wave around all the drops 
bool DominoSeq(double time, int *positions){
	bool sequenceIncomplete;
	
	
	return sequenceIncomplete;
}

// 2D wave pattern 
bool SurfaceWaveSeq(double time, int *positions){
	bool sequenceIncomplete;
	
	
	return sequenceIncomplete;
}



// Helper functions //

bool ResetDrops(int *positions, int xSize, int ySize){

	bool sequenceIncomplete = false;

	for(int counter = 0; counter < xSize*ySize; counter ++){
		if(positions[counter] > 0){
			positions[counter] = positions[counter] - 1;
			sequenceIncomplete = true;
		}
	}

	return sequenceIncomplete;

}

void shuffleArray(int* array, int size)
{

  for(int i = 0; i < (size); i++){
    array[i] = i;
  }

  int last = 0;
  int temp = array[last];

  for (int i=0; i<size; i++)
  {
    int index = random(size);
    array[last] = array[index];
    last = index;
  }
  array[last] = temp;
}

*/