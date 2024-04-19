#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

//  Define pin numbers for stepper motor CHANGE ALL PINS
const int stpPinHandDropStepper = 8;
const int dirPinHandDropStepper = 9;
const int enPinHandDropStepper = 13;

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

const int handDropVolumeUl = 647; // 0.65 mL
const int uLsPerRev = 422; // 3.56mL per 10
const int handDropSteps = 30000; //round((float)handDropVolumeUl / uLsPerRev * stepsPerRev);

const int bigTankVolume = 500; //2155; // MUST CHANGE! IN ML MUST CHECK. Setting at 500 for testing

const int mLsPerSecondSumpPump = 420; //  How many mLs per second the sump pump dispenses

// Define maximum speed and acceleration for the stepper motors
#define speedHandDropStepper 800 // 1000 steps per second
#define stepsPerRevHandDropStepper 200 // 200 steps per revolution
#define MICROSTEPS 1 // Microstepping for the stepper motors
const int handDropStepperRevs = 360; // Time to dispense the hand drop in milliseconds


// Define debounce interval for button
const int debounceInterval = 50; // Debounce interval for button

// Timeout
const int timeoutMillis = 20000; // Timeout duration in milliseconds, 20 seconds
// Other timing variables
const int tankDrainDuration = 9500; // Duration to drain the big tank
const int tankFillDuration = 5500; // Duration to fill the big tank

#endif // PINS_H