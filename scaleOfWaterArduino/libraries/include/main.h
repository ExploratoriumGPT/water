#ifndef MAIN_H
#define MAIN_H

// Main functions, in rough order of appearance
// setup functions
void setupButtons();
void setupSteppers();
void setupTank();
void pulseSSR(int pin); // Function to pulse the SSR so that it doesn't latch in the on position
// calibrate functions
void calibrateSumpPump();
// void calibratePump(AccelStepper stepper, bool forward);
//  main loop functions
void buttonPoll();
void stepperDispense();
void timeout();
void dispense();
void sumpPumpDispense();
void drainTank();
void overflowCheck(int tank); // Currently redundant with ISR, but may be useful in the future if we want a full check
void resolveOverflow(int tank);
void bigTankOverflowISR();
// helper functions
void gitPrint();
// Test functions
void testDrain();
void testSumpPump();
void testStepperDispense();

// Global variables declaration, more config variables in config.h
unsigned long lastButtonPressTime = 0;
unsigned long lastLittleDropDispense = 0;

// Flags
bool isTimeout = false; // Flag indicating if timeout has occurred

// typedef enum // Code to be used in the main loop to determine the state of the system
// {
//   HAND_DROP_STATE,
//   BIG_DROP_STATE,
//   RESET_STATE
// } State;

typedef enum
{
  HAND_DROP, // Note: not used but included for consistency with other enum
  LARGE_TANK,
  BOTH_TANKS
} DispenseType;

typedef enum
{
  EMPTY,
  DISPENSING,
  FULL,
  DRAINING,
  OVERFLOW
} TankState;

#endif // MAIN_H