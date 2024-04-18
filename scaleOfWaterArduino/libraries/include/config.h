#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

//  Define pin numbers for stepper motor CHANGE ALL PINS
const int stpPinHandDropStepper = 10;
const int dirPinHandDropStepper = 11;
const int enPinHandDropStepper = 15;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const int drainRelayPin = 42; // G-REL (32+8+3)
const int sumpPumpRelayPin = 23; // G-BRK (32+7)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// const int bigTankOverflowPin = 18;
// const int smallTankOverflowPin = 19;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Define pin number for button
#define NUM_BUTTONS 2
const int handDropButtonPin = 36; // G-BUT-3 P1 (32+4) (Called button 1 in the code)
const int bigDropButtonPin = 38; // G-BUT-3 P3 (32+6)
const uint8_t BUTTON_PINS[NUM_BUTTONS] = {handDropButtonPin, bigDropButtonPin};

// Define pin numbers for button lights
const int handDropButtonLightPin = 37; // G-BUT-3 P0 (32+5)
const int bigDropButtonLightPin = 39; // G-BUT-3 P2 (32+7)
const uint8_t BUTTON_LIGHT_PINS[NUM_BUTTONS] = {handDropButtonLightPin, bigDropButtonLightPin};

// CONFIG ITEMS, NOT PINS
// Define steps per revolution and mLs per revolution
const int stepsPerRev = 200;           // with no microstepping (200 steps per rev)

const int handDropVolume = 647; // 0.65 mL

const unsigned int smallTankVolume = 53895; // 53 mL
const int uLsPerRevSmallStepper = 422; // 3.56mL per 10

const int bigTankVolume = 500; //2155; // MUST CHANGE! IN ML MUST CHECK. Setting at 500 for testing
const int uLsPerRevBigStepper = 3190;   /////////////////////////////////////// 3,6mL per rev

const int mLsPerSecondSumpPump = 420; //  How many mLs per second the sump pump dispenses

// Define maximum speed and acceleration for the stepper motors
const float maxSpeedHandDropStepper = 560000; // 350*1600 per datasheet; NOTE: THIS IS OVER THE MAXIMUM VALUE FOR AN INT
const float maxAccelHandDropStepper = maxSpeedHandDropStepper;

// Define debounce interval for button
const int debounceInterval = 50; // Debounce interval for button

// Timeout
const int timeoutMillis = 20000; // Timeout duration in milliseconds, 20 seconds
// Other timing variables
const int tankDrainDuration = 9500; // Duration to drain the big tank
const int tankFillDuration = 5500; // Duration to fill the big tank

#endif // PINS_H