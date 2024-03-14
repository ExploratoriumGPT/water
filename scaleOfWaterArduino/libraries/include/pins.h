#ifndef PINS_H
#define PINS_H

//  Define pin numbers for stepper motor
//  https://blog.protoneer.co.nz/arduino-cnc-shield/ old shield
const int stepPinLittleStepper = 2; // X axis on board
const int dirPinLittleStepper = 5;
const int stepPinBigStepper = 4;
const int dirPinBigStepper = 7; // Z axis on board
const int enablePin = 8;
const int bigPumpEnablePin = 12;
#define drainBigTankRelayPin A3; // this is a define because Arduino.h isn't included in this file, but its same as other pin definitions

// Define pin number for button
const int buttonPin = 9; //"limit x axis on shield"
// const int buttonPin = 11; //"limit Z axis on shield"

#endif // PINS_H