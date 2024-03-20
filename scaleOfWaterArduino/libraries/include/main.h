#ifndef MAIN_H
#define MAIN_H

#include <AccelStepper.h> // Include the AccelStepper library. Necessary for declarations

void buttonSetup();
void buttonPoll();
void stepperDispense(AccelStepper stepper, long steps, bool dir, float uLsPerRev, float maxSpeed);
void setupAllSteppers();
void gitPrint();
void calibrateSumpPump();
void timeout();
void dispense();
void sumpPumpDispense(int mLs);
void drainTank();
void calibratePump(AccelStepper stepper, bool forward);

long millisToFillTank;
long millisAtStartOfFill;
long millisAtEndOfFill;
long lastButtonPressTime;

typedef enum // Code to be used in the main loop to determine the state of the system
{
  HAND_DROP_STATE,
  SMALL_DROP_STATE,
  BIG_DROP_STATE,
  RESET_STATE
} State;

typedef enum 
{
  HAND_DROP, // Note: not used
  SMALL_TANK,
  LARGE_TANK,
  BOTH_TANKS
} DispenseType;

#endif // MAIN_H