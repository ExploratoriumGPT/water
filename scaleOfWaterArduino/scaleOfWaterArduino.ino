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
#include "libraries/include/pins.h" // Header file for pin definitions

// Create instances of AccelStepper and Bounce objects
AccelStepper littleStepper(AccelStepper::DRIVER, stepPinLittleStepper, dirPinLittleStepper);
AccelStepper bigStepper(AccelStepper::DRIVER, stepPinBigStepper, dirPinBigStepper);
AccelStepper drainStepper(AccelStepper::DRIVER, stepPinDrainStepper, dirPinDrainStepper);

State state = RESET_STATE; // Initialize and set state
DispenseType dispenseType; // Initialize the dispense type

Bounce * buttons = new Bounce[NUM_BUTTONS]; // Create an array of button objects

void setup()
{
	Serial.begin(9600);
	Serial << F("All further serial debug will be at 115,200 baud") << endl;
	delay(1000);
	Serial.begin(115200);
	Serial << F("\nStarting setup") << endl;

	#ifdef PLATFORMIO
		gitPrint(); //prints git info to the serial monitor
	#endif

  drainTank(BOTH_TANKS); // Drain both tanks
	buttonSetup(); // Set up the buttons and button lights
	setupAllSteppers(); // Set up the stepper motors

	// Set up the button
	pinMode(bigPumpEnablePin, OUTPUT);
}

void loop()
{
	littleStepper.run();
	bigStepper.run();
	buttonPoll();
	dispense();

	if (!isTimeout && lastButtonPressTime > timeoutMillis)
	{
		Serial.println("TIME BASED timeout, draining tanks...");
		timeout();
	}

	// calibrateSumpPump();
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
		digitalWrite(BUTTON_LIGHT_PINS[i], ledState); // initialize the LED in high state
	}

}

void buttonPoll()
{    
	for (int i = 0; i < NUM_BUTTONS; i++)  {
		// Update the Bounce instance :
		buttons[i].update();
		// If it fell, flag the need to toggle the LED
		if ( buttons[i].fell() ) {
			if (i == NUM_BUTTONS - 1) {
				// Toggle the drain
				drainTank(BOTH_TANKS);
        Serial << "Drain button pressed" << endl;
			} else {
				// Toggle the LED
				ledState = LOW; // toggle the LED state off
        if (i == state) {
          drainTank(i); // if the button is pressed again, drain the tank
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
	drainStepper.setMaxSpeed(maxSpeedBigStepper);
	drainStepper.setSpeed(maxSpeedBigStepper);
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
} */

void timeout()
{
	Serial << "Timeout" << endl;
	// reset medium tank
	drainTank(BOTH_TANKS);
	isTimeout = true;
}

void dispense()
{
		Serial << "Button press, current state: " << state << endl;

    switch (state)
    {
    case HAND_DROP_STATE:
      Serial << "Hand drop state" << endl;
      stepperDispense(littleStepper, 100, true, uLsPerRevLittleStepper, maxSpeedLittleStepper);
      break;
    case SMALL_DROP_STATE:
      Serial << "Small tank drop state" << endl;
      stepperDispense(bigStepper, 100, true, uLsPerRevBigStepper, maxSpeedBigStepper);
      break;
    case BIG_DROP_STATE:
      Serial << "Big tank drop state" << endl;
      sumpPumpDispense(100);
      break;
    default:
      Serial << "Invalid state" << endl;
      break;
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


void drainTank(int tanksToDrain)
{
Serial << "Draining tank (3 is both) " << tanksToDrain << endl;
switch (tanksToDrain)
{
  case LARGE_TANK:
    digitalWrite(drainBigTankRelayPin, HIGH); // Turn on the relay to drain the big tank
    delay(tankDrainDuration); // Wait for the specified duration
    digitalWrite(drainBigTankRelayPin, LOW); // Turn off the relay to close the valve
    break;
  case SMALL_TANK:
    drainStepper.runSpeed(); // Run the stepper motor at the specified speed
    delay(tankDrainDuration); // Wait for the specified duration. Can change drain time to be smaller if necessary
    smallTankFull = false;
    break;
  case BOTH_TANKS:
    digitalWrite(drainBigTankRelayPin, HIGH); // Turn on the relay to drain the big tank
    drainStepper.runSpeed(); // Run the stepper motor at the specified speed
    delay(tankDrainDuration); // Wait for the specified duration
    // If small tank and large tank have separate drain times, use the drain time that is longer
    digitalWrite(drainBigTankRelayPin, LOW); // Turn off the relay to close the valve
    break;
  default:
    Serial << "Invalid tank to drain" << endl;
    break;
}
	state = RESET_STATE;
}
