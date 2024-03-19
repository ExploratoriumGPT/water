// Public libraries
#include <Arduino.h> // Library for Arduino
#include <AccelStepper.h> // Library for stepper motor
#include <Bounce2.h>     // Library for debouncing button
#include <EEPROM.h>     // Library for reading and writing to EEPROM
#include <Streaming.h> // Library for printing in streaming method
#ifdef PLATFORMIO
  #include <git_info.h>  // Library for printing git information
#endif
// Local files
#include "libraries/include/main.h" // Header file for scaleOfWaterArduino.ino
#include "libraries/include/pins.h" // Header file for pin definitions

// Create instances of AccelStepper and Bounce objects
AccelStepper littleStepper(AccelStepper::DRIVER, stepPinLittleStepper, dirPinLittleStepper);
AccelStepper bigStepper(AccelStepper::DRIVER, stepPinBigStepper, dirPinBigStepper);

#define STATE_ADDRESS 0

// Define steps per revolution and mLs per revolution
const int stepsPerRev = 1600;           // with 8x microstepping (200 steps per rev)
const int uLsPerRevLittleStepper = 422; // 3.56mL per 10
const int uLsPerRevBigStepper = 3190;   /////////////////////////////////////// 3,6mL per rev

const int mLsPerSecondSumpPump = 420; //  How many mLs per second the sump pump dispenses

// Define maximum speed and acceleration for the stepper motors
const int maxSpeedLittleStepper = 8 * 560000; // 350*1600 per datasheet;
const int maxAccelerationLittleStepper = maxSpeedLittleStepper;
const int maxSpeedBigStepper = maxSpeedLittleStepper;
const int maxAccelerationBigStepper = maxSpeedLittleStepper;

// Define debounce interval for button
const int debounceInterval = 50; // Debounce interval for button
const int timeoutMillis = 20000; // Timeout duration in milliseconds
const int bigTankDrainDuration = 4000; // Duration to drain the big tank
int state = 1; // Current state of the system
bool isTimeout = false; // Flag indicating if timeout has occurred
bool mediumTankFull = false; // Flag indicating if medium tank is full
bool bigTankFull = false; // Flag indicating if big tank is full

Bounce button = Bounce(); 

// Function to dispense X mL in the specified direction
void stepperDispense(AccelStepper stepper, long uL, bool forward, long uLsPerRev, int speed)
{
    digitalWrite(enablePin, LOW);
    // Calculate the number of steps required to dispense the specified uL
    int steps = round((float)uL / uLsPerRev * stepsPerRev);
    Serial << "Dispensing " << uL << " uL (" << steps << " steps) " << (forward ? "forwards... " : "backwards... ") << endl;
    stepper.move(forward ? -steps : steps); // Move the stepper motor forward or backward by the specified number of steps
    // Run the stepper motor until it reaches the target position ////blocking code
    while (stepper.distanceToGo() != 0)
    {
        stepper.run();
    }
    digitalWrite(enablePin, HIGH); // turn them off to save power
    Serial.println(" done");
}

void setupAllSteppers()
{
    pinMode(enablePin, OUTPUT);
    littleStepper.setMaxSpeed(maxSpeedLittleStepper);
    littleStepper.setAcceleration(maxAccelerationLittleStepper);
    bigStepper.setMaxSpeed(maxSpeedBigStepper);
    bigStepper.setAcceleration(maxAccelerationBigStepper);
}

void setup()
{
    #ifdef PLATFORMIO
      gitPrint(); //prints git info to the serial monitor
    #endif
    state = EEPROM.read(STATE_ADDRESS);

    // Set up the stepper motor
    Serial.begin(115200);
    Serial.println("Starting Stepper Demo...");
    setupAllSteppers();
    // Set up the button
    pinMode(buttonPin, INPUT_PULLUP);
    button.attach(buttonPin);
    button.interval(debounceInterval);
    pinMode(bigPumpEnablePin, OUTPUT);
}

// #DEFINE TINY_DROP_STATE 1
// #DEFINE BIG_DROP_STATE 2
// #DEFINE RESET_STATE 3
long millisToFillTank;
long millisAtStartOfFill;
long millisAtEndOfFill;
long lastButtonPressTime;


void loop()
{
    littleStepper.run();
    bigStepper.run();
    button.update();

    if (!isTimeout && lastButtonPressTime > timeoutMillis)
    {
        Serial.println("TIME BASED timeout, draining tank...");
        timeout();
    }
    // Check if the button is pressed and debounced
    if (button.fell())
    {
        lastButtonPressTime = millis();
        // calibratePump(bigStepper, false);
        // calibratePump(littleStepper, true);
        // calibratePump(littleStepper, true);
        // calibratePump(littleStepper, true);
        //
        exhibitRoutine();
    }
    // calibrateSumpPump();
}

#ifdef PLATFORMIO
void gitPrint() { //prints git information to the serial monitor using the Streaming library
  Serial << F("Git Information:\n")
  << F("Build Date/Time (local time): ") << BUILD_DATE << F("\n")
  << F("Builder's Name:  ") << GIT_USER_NAME << F(" Email: ") << GIT_USER_EMAIL  << F("\n")
  << F("Repository URL: ") << GIT_REPO_URL << F("\n")
  << F("Branch: ") << GIT_BRANCH << F(" | Tag: ") << GIT_TAG  << F("\n\n")
  << F("Commit Hash: ") << GIT_COMMIT_HASH << F("\n") 
  << F("\n=================================") << endl;
}
#endif

void calibrateSumpPump()
{
    if (button.fell())
    {
        digitalWrite(bigPumpEnablePin, HIGH);
        millisAtStartOfFill = millis();
        Serial << "button down!" << endl;
    }
    else if (button.rose())
    {
        digitalWrite(bigPumpEnablePin, LOW);
        millisAtEndOfFill = millis();
        millisToFillTank = millisAtEndOfFill - millisAtStartOfFill;
        Serial << "millisToFillTank: " << millisToFillTank << endl;
    }
}

void timeout()
{
    Serial << "Timeout, draining tank..." << endl;
    // reset medium tank
    stepperDispense(bigStepper, 53895, false, uLsPerRevBigStepper, maxSpeedBigStepper); ///////////////////////////
    Serial << "Medium tank empty" << endl;
    digitalWrite(bigPumpEnablePin, HIGH);
    delay(bigTankDrainDuration);
    digitalWrite(bigPumpEnablePin, LOW);
    Serial << "big tank empty" << endl;
    state = 0;
    EEPROM.write(STATE_ADDRESS, state);
    isTimeout = true;
    mediumTankFull = false;
    bigTankFull = false;
}

void exhibitRoutine()
{
    {
        Serial << "Button pressed!";
        Serial << "Current state: " << state << endl;

        if (state == 1)
        {
            if (mediumTankFull) // if there is still water in the medium tank then timeout to drain
            {
                Serial << " BUTTON BASED RESET, DRAINING TANK";
                timeout();
                state = 0;
            }
            else
            {

                // isTimeout = false;
                // sumpPumpDispense(2155);
            }
        }
        else if (state == 2)
        {
            // spurt 53mL into the little tank
            stepperDispense(bigStepper, 53895, true, uLsPerRevBigStepper, maxSpeedBigStepper);
            mediumTankFull = true;
                Serial << "bigStepper drip dropped" << endl;
            }
            else if (state == 3)
            {
                // spurt 0.65mL into user's hands
                stepperDispense(littleStepper, 647, true, uLsPerRevLittleStepper, maxSpeedLittleStepper);
                Serial << "littleStepper drip dropped" << endl;
                state = 0;
        }

        state = state + 1;
        EEPROM.write(STATE_ADDRESS, state);
    }
}

void sumpPumpDispense(int mLs)
{
    Serial << "Dispensing " << mLs << " mLs sump pump... " << endl;
    digitalWrite(bigPumpEnablePin, HIGH);
    delay(mLs / mLsPerSecondSumpPump * 1000);
    digitalWrite(bigPumpEnablePin, LOW);
    Serial << "Done Pumping Sump Pump" << endl;
    bigTankFull = true;
}

void calibratePump(AccelStepper stepper, bool forward)
{
    int revs = 10;
    int steps = revs * stepsPerRev;
    Serial << "Calibrating pump... " << "spinning " << revs << " revs (" << steps << " steps) " 
    << (forward ? "forwards... " : "backwards... ") << endl;
    digitalWrite(enablePin, LOW);
    stepper.move(forward ? -steps : steps);
    while (stepper.distanceToGo() != 0)
    {
        stepper.run();
    }
    digitalWrite(enablePin, HIGH); // turn them off to save power
    Serial << " done. note volume dispensed and divide by 10 to get uL per rev" << endl;
}
