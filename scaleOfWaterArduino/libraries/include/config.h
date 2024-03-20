#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

//  Define pin numbers for stepper motor CHANGE ALL PINS
const int stpPinHandDropStepper = 2; //
const int dirPinHandDropStepper = 5;
const int enPinHandDropStepper = 13;

const int stpPinSmallTankStepper = 4;
const int dirPinSmallTankStepper = 7; //
const int enPinSmallTankStepper = 8; //

const int stpPinDrainStepper = 3; //
const int dirPinDrainStepper = 6; //
const int enPinDrainStepper = 12;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const int drainBigTankRelayPin = A3; // this is a define because Arduino.h isn't included in this file, but its same as other pin definitions
const int sumpPumpRelayPin = A5; // CHANGE PIN
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const int bigTankOverflowPin = A6; // CHANGE PIN
const int smallTankOverflowPin = A7; // CHANGE PIN
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Define pin number for button
#define NUM_BUTTONS 4
const int bigDropButtonPin = 9; // CHANGE PIN
const int smallDropButtonPin = 10; // CHANGE PIN
const int handDropButtonPin = 11; // CHANGE PIN
const int drainButtonPin = A4; // CHANGE PIN (no LED) (Interrupt?)
const uint8_t BUTTON_PINS[NUM_BUTTONS] = {bigDropButtonPin, smallDropButtonPin, handDropButtonPin, drainButtonPin};

// Define pin numbers for button lights
#define NUM_BUTTON_LIGHTS 3
const int bigDropButtonLightPin = A0; // CHANGE PIN
const int smallDropButtonLightPin = A1; // CHANGE PIN
const int handDropButtonLightPin = A2; // CHANGE PIN
const uint8_t BUTTON_LIGHT_PINS[NUM_BUTTON_LIGHTS] = {bigDropButtonLightPin, smallDropButtonLightPin, handDropButtonLightPin};

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
const int maxSpeedHandDropStepper = 8 * 560000; // 350*1600 per datasheet; NOTE: THIS IS OVER THE MAXIMUM VALUE FOR AN INT
const int maxAccelHandDropStepper = maxSpeedHandDropStepper;
const int maxSpeedSmallTankStepper = maxSpeedHandDropStepper;
const int maxAccelSmallTankStepper = maxSpeedHandDropStepper;
const int maxSpeedDrainStepper = maxSpeedHandDropStepper; // no acceleration for drain stepper

// Define debounce interval for button
const int debounceInterval = 50; // Debounce interval for button

// Timeout
const int timeoutMillis = 20000; // Timeout duration in milliseconds
bool isTimeout = false; // Flag indicating if timeout has occurred
// Other timing variables
unsigned long previousMillis = 0; // Variable to store the last time the button was pressed
const int tankDrainDuration = 4000; // Duration to drain the big tank

#endif // PINS_H