#include "helper_functions.h"
#include "config.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "string.h"
#include "esp_log.h"
#include "esp_err.h"


// Helper functions for the main loop

void zeroPositionArrays(int *positions, int *nextPositions, int xsize, int ysize){
  for (int x = 0; x < xsize; x++){
    for (int y = 0; y < ysize; y++){
      positions[x * ysize + y] = 0;
      nextPositions[x * ysize + y] = 0;
    }
  }
}

// Function to set or clear the n-th bit of a character to the value of x. 
void setBit(unsigned char *mychar, int n, int x) {
  if (x == 1) {
      // Set the n-th bit to 1
      *mychar |= (1 << n);
  } else {
      // Clear the n-th bit to 0
      *mychar &= ~(1 << n);
  }
}

void removeStepBits(unsigned char *byteArr, int size){
  for(int i = 0; i < size; i++){
    setBit(byteArr+i, 1, 0);
    setBit(byteArr+i, 3, 0);
    setBit(byteArr+i, 5, 0);
    setBit(byteArr+i, 7, 0);
  }
}


  //Initialize collections bytes
  // for (int x = 0; x < xsize; x++){
  //   for (int y = 0; y < ysize; y++){
  //     collections[x][y] = 0;
  //   }
  // }
  
  // while(incomplete){

  //   incomplete = false;
  //   // Write the bits into the collection(s) of bytes
  //   for (int x = 0; x < x_size; x++) {
  //     for (int y = 0; y < y_size; y++) {

  //       // Formula for selecting the correct step and dir bit in the collection
  //       stepBit = 2 * (2 * (x % 2) + (y % 2)) + 1;
  //       dirBit = 2 * (2 * (x % 2) + (y % 2));

  //       if(positions[x][y] < nextPositions[x][y]){
  //         positions[x][y] = positions[x][y] + 1;
  //         incomplete = true;
  //         bitWrite(collections[x/2][y/2], stepBit, 1);
  //         bitWrite(collections[x/2][y/2], dirBit, 0);
  //       }
        
  //       if(positions[x][y] > nextPositions[x][y]){
  //         positions[x][y] = positions[x][y] - 1;
  //         incomplete = true;
  //         bitWrite(collections[x/2][y/2], stepBit, 1);
  //         bitWrite(collections[x/2][y/2], dirBit, 1);
  //       }

  //     }
  //   }

  //   //Send the steps needed using each byte in collections

  //   startMicros = micros();

  //   for(int x = 0; x < xColSize; x++){
  //     for(int y = 0; y < yColSize; y++){
        
  //       int currReg = x*yColSize + y;

  //       updateShiftRegister(
  //         registers[currReg][0], registers[currReg][1], 
  //         registers[currReg][2],  collections[x][y]);

  //     }
  //   }

  //   while(micros() - startMicros < minInterval){
  //     continue;
  //   }

  //   for(int x = 0; x < xColSize; x++){
  //     for(int y = 0; y < yColSize; y++){
  //       // Set move bits to 0
  //       bitWrite(collections[x][y], 7, 0);
  //       bitWrite(collections[x][y], 5, 0);
  //       bitWrite(collections[x][y], 3, 0);
  //       bitWrite(collections[x][y], 1, 0);

  //       int currReg = x*yColSize + y;

  //       updateShiftRegister(
  //         registers[currReg][0], registers[currReg][1], 
  //         registers[currReg][2],  collections[x][y]);

  //     }
  //   }
  // }

// void updateShiftRegister(int dataPin, int latchPin, int clockPin, byte regVal)
// {
//   //registers refer to the register pin 
//   digitalWrite(latchPin, LOW);
//   shiftOut(dataPin, clockPin, LSBFIRST, regVal);
//   digitalWrite(latchPin, HIGH);
//   return;
// }

// void interruptFunc(){
//   if(millis()-last_interrupt_time > 500){
//     interruptEnable = !interruptEnable;
//     Serial.println("Interrupted");
//   }
//   last_interrupt_time = millis();
// }
