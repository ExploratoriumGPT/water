#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

//  Define pin numbers for stepper motor CHANGE ALL PINS
const int stpPinHandDropStepper = 10;
const int dirPinHandDropStepper = 11;
const int enPinHandDropStepper = 15;

const int stpPinSmallTankStepper = 2;
const int dirPinSmallTankStepper = 3;
const int enPinSmallTankStepper = 7;

const int stpPinDrainStepper = 8;
const int dirPinDrainStepper = 9;
const int enPinDrainStepper = 13;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const int drainBigTankRelayPin = 23;
const int sumpPumpRelayPin = 43;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// const int bigTankOverflowPin = 18;
// const int smallTankOverflowPin = 19;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Define pin number for button
#define NUM_BUTTONS 4
const int handDropButtonPin = 33;
const int smallDropButtonPin = 35;
const int bigDropButtonPin = 37;
const int drainButtonPin = 39;
const uint8_t BUTTON_PINS[NUM_BUTTONS] = {handDropButtonPin, smallDropButtonPin, bigDropButtonPin, drainButtonPin};

// Define pin numbers for button lights
#define NUM_BUTTON_LIGHTS 3
const int handDropButtonLightPin = 32;
const int smallDropButtonLightPin = 34;
const int bigDropButtonLightPin = 36;
const uint8_t BUTTON_LIGHT_PINS[NUM_BUTTON_LIGHTS] = {smallDropButtonLightPin, bigDropButtonLightPin, handDropButtonLightPin};

// CONFIG ITEMS, NOT PINS
// Define steps per revolution and mLs per revolution
const int stepsPerRev = 1600;           // with 8x microstepping (200 steps per rev)

const int handDropVolume = 647; // 0.65 mL

const unsigned int smallTankVolume = 53895; // 53 mL
const int uLsPerRevSmallStepper = 422; // 3.56mL per 10

const int bigTankVolume = 500; //2155; // MUST CHANGE! IN ML MUST CHECK. Setting at 500 for testing
const int uLsPerRevBigStepper = 3190;   /////////////////////////////////////// 3,6mL per rev

const int mLsPerSecondSumpPump = 420; //  How many mLs per second the sump pump dispenses

// Define maximum speed and acceleration for the stepper motors
const float maxSpeedHandDropStepper = 560000; // 350*1600 per datasheet; NOTE: THIS IS OVER THE MAXIMUM VALUE FOR AN INT
const float maxAccelHandDropStepper = maxSpeedHandDropStepper;
const float maxSpeedSmallTankStepper = maxSpeedHandDropStepper;
const float maxAccelSmallTankStepper = maxSpeedHandDropStepper;
const float maxSpeedDrainStepper = maxSpeedHandDropStepper; // no acceleration for drain stepper

// Define debounce interval for button
const int debounceInterval = 50; // Debounce interval for button

// Timeout
const int timeoutMillis = 20000; // Timeout duration in milliseconds, 20 seconds
// Other timing variables
const int tankDrainDuration = 4000; // Duration to drain the big tank

#endif // PINS_H