#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

//  Define pin numbers for stepper motor
const int stepPinLittleStepper = 2; // X axis on board
const int dirPinLittleStepper = 5;
const int stepPinBigStepper = 4;
const int dirPinBigStepper = 7; // Z axis on board
const int stepPinDrainStepper = 3; // CHANGE PIN
const int dirPinDrainStepper = 6; // CHANGE PIN
const int enablePin = 8;
const int bigPumpEnablePin = 12;
const int drainBigTankRelayPin = A3; // this is a define because Arduino.h isn't included in this file, but its same as other pin definitions

// Define pin number for button
#define NUM_BUTTONS 4
const int bigDropButtonPin = 9; // CHANGE PIN
const int smallDropButtonPin = 10; // CHANGE PIN
const int handDropButtonPin = 11; // CHANGE PIN
const int drainButtonPin = A4; // CHANGE PIN (no LED)
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
const int uLsPerRevLittleStepper = 422; // 3.56mL per 10
const int uLsPerRevBigStepper = 3190;   /////////////////////////////////////// 3,6mL per rev

const int mLsPerSecondSumpPump = 420; //  How many mLs per second the sump pump dispenses

// Define maximum speed and acceleration for the stepper motors
const int maxSpeedLittleStepper = 8 * 560000; // 350*1600 per datasheet; NOTE: THIS IS OVER THE MAXIMUM VALUE FOR AN INT
const int maxAccelerationLittleStepper = maxSpeedLittleStepper;
const int maxSpeedBigStepper = maxSpeedLittleStepper;
const int maxAccelerationBigStepper = maxSpeedLittleStepper;
const int maxSpeedDrainStepper = maxSpeedLittleStepper;

// Define debounce interval for button
const int debounceInterval = 50; // Debounce interval for button
const int timeoutMillis = 20000; // Timeout duration in milliseconds
const int tankDrainDuration = 4000; // Duration to drain the big tank
bool isTimeout = false; // Flag indicating if timeout has occurred
bool smallTankFull = false; // Flag indicating if medium tank is full
bool bigTankFull = false; // Flag indicating if big tank is full

#endif // PINS_H