/*

*/

#include <AccelStepper.h>
#include <Bounce2.h>
#include <EEPROM.h>

#define STATE_ADDRESS 0
//  Define pin numbers for stepper motor
//  https://blog.protoneer.co.nz/arduino-cnc-shield/ old shield
const int stepPinLittleStepper = 2; // X axis on board
const int dirPinLittleStepper = 5;
const int stepPinBigStepper = 4;
const int dirPinBigStepper = 7; // Z axis on board
const int enablePin = 8;
const int bigPumpEnablePin = 12;
const int drainBigTankRelayPin = A3;

// Define pin number for button
const int buttonPin = 9; //"limit x axis on shield"
// const int buttonPin = 11; //"limit Z axis on shield"

// Define steps per revolution and mLs per revolution
const int stepsPerRev = 1600;           // with 8x microstepping (200 steps per rev)
const int uLsPerRevLittleStepper = 422; // 3.56mL per 10
const int uLsPerRevBigStepper = 3190;   /////////////////////////////////////// 3,6mL per rev

const int mLsPerSecondSumpPump = 420;

// Define maximum speed and acceleration
const int maxSpeedLittleStepper = 8 * 560000; // 350*1600 per datasheet;
const int maxAccelerationLittleStepper = maxSpeedLittleStepper;
const int maxSpeedBigStepper = maxSpeedLittleStepper;
const int maxAccelerationBigStepper = maxSpeedLittleStepper;

// Define debounce interval for button
const int debounceInterval = 50;
const int timeoutMillis = 20000;
const int bigTankDrainDuration = 4000;
int state = 1;
bool isTimeout = false;
bool mediumTankFull = false;
bool bigTankFull = false;
// Create instances of AccelStepper and Bounce objects
AccelStepper littleStepper(AccelStepper::DRIVER, stepPinLittleStepper, dirPinLittleStepper);
AccelStepper bigStepper(AccelStepper::DRIVER, stepPinBigStepper, dirPinBigStepper);

Bounce button = Bounce();

// Function to dispense X mL in the specified direction
void stepperDispense(AccelStepper stepper, long uL, bool forward, long uLsPerRev, int speed)
{
    digitalWrite(enablePin, LOW);
    // Calculate the number of steps required to dispense the specified uL
    int steps = round((float)uL / uLsPerRev * stepsPerRev);
    Serial.print("Dispensing ");
    Serial.print(uL);
    Serial.print(" uL (");
    Serial.print(steps);
    Serial.print(" steps) ");
    Serial.print(forward ? "forwards... " : "backwards... ");
    stepper.move(forward ? -steps : steps);
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

void calibrateSumpPump()
{
    if (button.fell())
    {
        digitalWrite(bigPumpEnablePin, HIGH);
        millisAtStartOfFill = millis();
        Serial.println("button down!");
    }
    else if (button.rose())
    {
        digitalWrite(bigPumpEnablePin, LOW);
        millisAtEndOfFill = millis();
        millisToFillTank = millisAtEndOfFill - millisAtStartOfFill;
        Serial.print("millisToFillTank: ");
        Serial.println(millisToFillTank);
    }
}

void timeout()
{
    Serial.print("timeout, draining tank...");
    // reset medium tank
    stepperDispense(bigStepper, 53895, false, uLsPerRevBigStepper, maxSpeedBigStepper); ///////////////////////////
    Serial.print("medium tank empty");
    digitalWrite(bigPumpEnablePin, HIGH);
    delay(bigTankDrainDuration);
    digitalWrite(bigPumpEnablePin, LOW);
    Serial.print("big tank empty");
    state = 0;
    EEPROM.write(STATE_ADDRESS, state);
    isTimeout = true;
    mediumTankFull = false;
    bigTankFull = false;
}

void exhibitRoutine()
{
    {
        Serial.print("Button pressed!");
        Serial.print("Current state: ");
        Serial.println(state);

        if (state == 1)
        {
            if (mediumTankFull) // if there is still water in the medium tank then timeout to drain
            {
                Serial.print(" BUTTON BASED RESET, DRAINING TANK");
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
            Serial.println("bigStepper drip dropped");
        }
        else if (state == 3)
        {
            // spurt 0.65mL into user's hands
            stepperDispense(littleStepper, 647, true, uLsPerRevLittleStepper, maxSpeedLittleStepper);
            Serial.println("littleStepper drip dropped");
            state = 0;
        }

        state = state + 1;
        EEPROM.write(STATE_ADDRESS, state);
    }
}

void sumpPumpDispense(int mLs)
{
    Serial.print("Dispensing ");
    Serial.print(mLs);
    Serial.print(" mLs sump pump... ");
    digitalWrite(bigPumpEnablePin, HIGH);
    delay(mLs / mLsPerSecondSumpPump * 1000);
    digitalWrite(bigPumpEnablePin, LOW);
    Serial.println(" done");
    bigTankFull = true;
}

void calibratePump(AccelStepper stepper, bool forward)
{
    int revs = 10;
    int steps = revs * stepsPerRev;
    Serial.print("Calibrating pump... ");
    Serial.print("spinning ");
    Serial.print(revs);
    Serial.print(" revs (");
    Serial.print(steps);
    Serial.print(" steps) ");
    Serial.print(forward ? "forwards... " : "backwards... ");
    digitalWrite(enablePin, LOW);
    stepper.move(forward ? -steps : steps);
    while (stepper.distanceToGo() != 0)
    {
        stepper.run();
    }
    digitalWrite(enablePin, HIGH); // turn them off to save power
    Serial.println(" done. note volume dispensed and divide by 10 to get uL per rev");
}
