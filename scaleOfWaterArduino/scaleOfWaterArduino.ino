// Public libraries
#include <Arduino.h> // Library for Arduino
#include <AccelStepper.h> // Library for stepper motor
#include <Bounce2.h>     // Library for debouncing button
#include <Streaming.h> // Library for printing in streaming method
#ifdef PLATFORMIO
  #include <git_info.h>  // Library for printing git information
#endif
// Local files
#include "libraries/include/main.h" // Header file for scaleOfWaterArduino.ino
#include "libraries/include/config.h" // Header file for pin definitions and configuration items

// Create instances of AccelStepper and Bounce objects
AccelStepper handDropStepper(AccelStepper::DRIVER, stpPinHandDropStepper, dirPinHandDropStepper);
AccelStepper smallTankStepper(AccelStepper::DRIVER, stpPinSmallTankStepper, dirPinSmallTankStepper);
AccelStepper drainStepper(AccelStepper::DRIVER, stpPinDrainStepper, dirPinDrainStepper);
Bounce * buttons = new Bounce[NUM_BUTTONS]; // Create an array of button objects

// Initialize enums
State state = RESET_STATE; // Initialize enum and set state
DispenseType dispenseType; // Initialize the dispense enum

void setup()
{
	Serial.begin(9600);
	Serial << F("All further serial debug will be at 115,200 baud") << endl;
	delay(1000);
	Serial.begin(115200);
	Serial << F("Starting setup") << endl;

	#ifdef PLATFORMIO
		gitPrint(); //prints git info to the serial monitor
	#endif

  drainTank(BOTH_TANKS); // Drain both tanks
	buttonSetup(); // Set up the buttons and button lights
	setupAllSteppers(); // Set up the stepper motors
}

void loop()
{
	handDropStepper.run();
	smallTankStepper.run();
	buttonPoll();
	dispense();
	timeout();
}

void buttonSetup()
{
	// Setup button pins
	for (int i = 0; i < NUM_BUTTONS; i++) {
		buttons[i].attach( BUTTON_PINS[i] , INPUT_PULLUP  );       //setup the bounce instance for the current button
		buttons[i].interval(debounceInterval);              // interval in ms
	}

	//Setup LED pins
	for (int i = 0; i < NUM_BUTTON_LIGHTS; i++) {
		pinMode(BUTTON_LIGHT_PINS[i], OUTPUT); // set the LED pin to output
		digitalWrite(BUTTON_LIGHT_PINS[i], HIGH); // initialize the LED in high state
	}

}

// Function to set up all stepper motors for AccelStepper library
void setupAllSteppers()
{
	handDropStepper.setEnablePin(enPinHandDropStepper); // Set enable pin for little stepper motor
	handDropStepper.setMaxSpeed(maxSpeedHandDropStepper); // Set maximum speed for little stepper motor
	handDropStepper.setAcceleration(maxAccelHandDropStepper); // Set acceleration for little stepper motor

	smallTankStepper.setEnablePin(enPinSmallTankStepper); // Set enable pin for big stepper motor
	smallTankStepper.setMaxSpeed(maxSpeedSmallTankStepper); // Set maximum speed for big stepper motor
	smallTankStepper.setAcceleration(maxAccelSmallTankStepper); // Set acceleration for big stepper motor

	drainStepper.setEnablePin(enPinDrainStepper); // Set enable pin for drain stepper motor
	drainStepper.setMaxSpeed(maxSpeedDrainStepper); // Set maximum speed for drain stepper motor
	drainStepper.setSpeed(maxSpeedDrainStepper); // Set speed for drain stepper motor
}

/* Commenting out for now to worry about calibration later
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
*/

void buttonPoll()
{    
	for (int i = 0; i < NUM_BUTTONS; i++)  {
		// Update the Bounce instance :
		buttons[i].update();
		// If it fell, flag the need to toggle the LED
		if ( buttons[i].fell() ) {
			if (i == NUM_BUTTONS - 1) { // Make sure drain button pin is the last one in the array
				// Toggle the drain
				drainTank(BOTH_TANKS);
        Serial << "Drain button pressed" << endl;
			} else {
				// Toggle the LED
				digitalWrite(BUTTON_LIGHT_PINS[i], LOW);
        if (i == state) {
          drainTank(i); // if the button is pressed again, drain the tank
          digitalWrite(BUTTON_LIGHT_PINS[i], HIGH); // Turn on the light for the button
        }
				state = static_cast<State>(i); // set the state to the button that was pressed
        lastButtonPressTime = millis(); // update the last button press time for timeout
			}
		}
	}

}

// Function to dispense X mL in the specified direction
void stepperDispense(AccelStepper stepper, long uL, bool forward, long uLsPerRev, int speed)
{
	stepper.enableOutputs(); // Enable the stepper motor outputs
	// Calculate the number of steps required to dispense the specified uL
	int steps = round((float)uL / uLsPerRev * stepsPerRev);
	Serial << "Dispensing " << uL << " uL (" << steps << " steps) " << (forward ? "forwards... " : "backwards... ") << endl;
	stepper.move(forward ? -steps : steps); // Move the stepper motor forward or backward by the specified number of steps
	// Run the stepper motor until it reaches the target position ////blocking code
	while (stepper.distanceToGo() != 0)
	{
		stepper.run();
	}
	stepper.disableOutputs(); // Disable the stepper motor outputs
	Serial.println(" done");
}

void timeout()
{
	if (lastButtonPressTime > timeoutMillis)
	{
		Serial << "Timeout" << endl;
		drainTank(BOTH_TANKS);
	}
}

void dispense()
{
		Serial << "Button press, current state: " << state << endl;

    switch (state)
    {
    case HAND_DROP_STATE:
      Serial << "Hand drop state" << endl;
      stepperDispense(handDropStepper, handDropVolume, true, uLsPerRevSmallStepper, maxSpeedHandDropStepper);
      digitalWrite(BUTTON_LIGHT_PINS[HAND_DROP], HIGH); // Turn on the light for the hand drop button after dispensing
      break;
    case SMALL_DROP_STATE:
      digitalWrite(BUTTON_LIGHT_PINS[SMALL_TANK], LOW); // Turn off the light for the small tank button to show that its full
      Serial << "Small tank drop state" << endl;
      stepperDispense(smallTankStepper, smallTankVolume, true, uLsPerRevBigStepper, maxSpeedSmallTankStepper);
      break;
    case BIG_DROP_STATE:
      digitalWrite(BUTTON_LIGHT_PINS[LARGE_TANK], LOW); // Turn off the light for the large tank button to show that its full
      Serial << "Big tank drop state" << endl;
      sumpPumpDispense(bigTankVolume);
      break;
    default:
      Serial << "Invalid state" << endl;
      break;
	}
}

void sumpPumpDispense(int mLs)
{
	Serial << "Dispensing " << mLs << " mLs sump pump... " << endl;
	digitalWrite(sumpPumpRelayPin, HIGH);
	delay(mLs / mLsPerSecondSumpPump * 1000);
	digitalWrite(sumpPumpRelayPin, LOW);
	Serial << "Done Pumping Sump Pump" << endl;
}

void drainTank(int tanksToDrain)
{
  Serial << "Draining tank (3 is both) " << tanksToDrain << endl;
	drainStepper.enableOutputs(); // Enable the stepper motor outputs for the drain stepper
  switch (tanksToDrain)
  {
    case LARGE_TANK:
      digitalWrite(drainBigTankRelayPin, HIGH); // Turn on the relay to drain the big tank
      delay(tankDrainDuration); // Wait for the specified duration
      digitalWrite(drainBigTankRelayPin, LOW); // Turn off the relay to close the valve
			//overflowCheck(LARGE_TANK); // Check for overflow after draining the tank. Currently redundant with ISR, but may be useful in the future if we want a full check
			digitalWrite(BUTTON_LIGHT_PINS[LARGE_TANK], HIGH); // Turn on the light for the small tank button
      break;
    case SMALL_TANK:
      drainStepper.runSpeed(); // Run the stepper motor at the specified speed
      delay(tankDrainDuration); // Wait for the specified duration. Can change drain time to be smaller if necessary
			//overflowCheck(SMALL_TANK); // Check for overflow after draining the tank. Currently redundant with ISR, but may be useful in the future if we want a full check
      digitalWrite(BUTTON_LIGHT_PINS[SMALL_TANK], HIGH); // Turn on the light for the small tank button
      break;
    case BOTH_TANKS:
      digitalWrite(drainBigTankRelayPin, HIGH); // Turn on the relay to drain the big tank
      drainStepper.runSpeed(); // Run the stepper motor at the specified speed
      delay(tankDrainDuration); // Wait for the specified duration
      // If small tank and large tank have separate drain times, use the drain time that is longer
      digitalWrite(drainBigTankRelayPin, LOW); // Turn off the relay to close the valve
			//overflowCheck(BOTH_TANKS); // Check for overflow after draining the tank. Currently redundant with ISR, but may be useful in the future if we want a full check
      digitalWrite(BUTTON_LIGHT_PINS[LARGE_TANK], HIGH); // Turn on the light for the small tank button
      digitalWrite(BUTTON_LIGHT_PINS[SMALL_TANK], HIGH); // Turn on the light for the small tank button
      break;
    default:
      Serial << "Invalid tank to drain" << endl;
      break;
  }
	drainStepper.disableOutputs(); // Disable the stepper motor outputs for the drain stepper
	state = RESET_STATE;
}

void overflowCheck(int tank) { // Currently redundant with ISR, but may be useful in the future if we want a full check
    bool overflowDetected = false;

    // Check for overflow based on the tank type
    if (tank == LARGE_TANK || tank == BOTH_TANKS) {
        if (digitalRead(bigTankOverflowPin)) {
            Serial << "Big tank overflow detected." << endl;
            overflowDetected = true;
        }
    }

    if (tank == SMALL_TANK || tank == BOTH_TANKS) {
        if (digitalRead(smallTankOverflowPin)) {
            Serial << "Small tank overflow detected." << endl;
            overflowDetected = true;
        }
    }

    // If an overflow is detected, attempt to resolve it
    if (overflowDetected) {
      resolveOverflow(tank);
    }
}

void resolveOverflow(int tank) {
	const int maxRetries = 5;
	const unsigned long delayDuration = 10000; // 10 seconds for retry delay

	for (int retry = 0; retry < maxRetries; ++retry) {
			drainTank(tank);  // Attempt to drain the tank(s)
			delay(delayDuration);  // Wait before rechecking

			// Recheck for overflow condition
			if (!(tank == LARGE_TANK || tank == BOTH_TANKS) || !digitalRead(bigTankOverflowPin)) {
					if (!(tank == SMALL_TANK || tank == BOTH_TANKS) || !digitalRead(smallTankOverflowPin)) {
							Serial << "Overflow resolved after " << (retry + 1) << " attempts." << endl;
							return; // Overflow successfully resolved
					}
			}
	}
	Serial << "Overflow could not be resolved after " << maxRetries << " attempts. Stopping program." << endl;
	while(1) {
		Serial << "Program Stop due to overflow" << endl;
		delay(5000);
	}
}

void overflowBigTankISR() {
	Serial << "Big tank overflow detected." << endl;
	resolveOverflow(LARGE_TANK);
}

void overflowSmallTankISR() {
	Serial << "Small tank overflow detected." << endl;
	resolveOverflow(SMALL_TANK);
}

#ifdef PLATFORMIO //if using platformio, print git information
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
