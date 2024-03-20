#ifndef MAIN_H
#define MAIN_H

#include <AccelStepper.h> // Include the AccelStepper library. Necessary for declarations

// Main functions, in rough order of appearance
// setup functions
void buttonSetup();
void setupAllSteppers();
// calibrate functions
void calibrateSumpPump();
void calibratePump(AccelStepper stepper, bool forward);
// main loop functions
void buttonPoll();
void stepperDispense(AccelStepper stepper, long steps, bool dir, float uLsPerRev, float maxSpeed);
void timeout();
void dispense();
void sumpPumpDispense(int mLs);
void drainTank();
void overflowCheck(int tank); // Currently redundant with ISR, but may be useful in the future if we want a full check
void resolveOverflow(int tank);
void bigTankOverflowISR();
void smallTankOverflowISR();
// helper functions
void gitPrint();

// long millisToFillTank;
// long millisAtStartOfFill;
// long millisAtEndOfFill;
long lastButtonPressTime = 0;

typedef enum // Code to be used in the main loop to determine the state of the system
{
  HAND_DROP_STATE,
  SMALL_DROP_STATE,
  BIG_DROP_STATE,
  RESET_STATE
} State;

typedef enum 
{
  HAND_DROP, // Note: not used but included for consistency with other enum
  SMALL_TANK,
  LARGE_TANK,
  BOTH_TANKS
} DispenseType;

#endif // MAIN_H