// Public libraries
#include <Arduino.h> // Library for Arduino
#include <AccelStepper.h> // Library for stepper motor
#include <Bounce2.h>     // Library for debouncing button
#include <Streaming.h> // Library for printing in streaming method
#ifdef PLATFORMIO
  #include <git_info.h>  // Library for printing git information
#endif
// Local files. May be confusion because they are located in libraries folder, but this is because this is how arduino IDE needs it
#include "libraries/include/main.h" // Header file for scaleOfWaterArduino.ino
#include "libraries/include/config.h" // Header file for pin definitions and configuration items

// Create instances of AccelStepper and Bounce objects
AccelStepper handDropStepper(AccelStepper::DRIVER, stpPinHandDropStepper, dirPinHandDropStepper);
Bounce * buttons = new Bounce[NUM_BUTTONS]; // Create an array of button objects

// Initialize enums
State state = RESET_STATE; // Initialize enum and set state
DispenseType dispenseType; // Initialize the dispense enum
TankState largeTankState = IDLE; // Initialize the large tank state

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

  drainTank(); // Drain both tanks
	setupButtons(); // Set up the buttons and button lights
	setupSteppers(); // Set up the stepper motors
	pinMode(drainRelayPin, OUTPUT); // Set the relay pin to output
	pinMode(sumpPumpRelayPin, OUTPUT); // Set the relay pin to output
}

void loop()
{
	//buttonPoll();
	//drainTank();
	//delay(5000);
	//dispense();
	//timeout();
	testDrain();
	//testStepperDispense();
}

void setupButtons()
{
	// Setup button pins
	for (int i = 0; i < NUM_BUTTONS; i++) {
		buttons[i].attach(BUTTON_PINS[i], INPUT_PULLUP);       //setup the bounce instance for the current button
		buttons[i].interval(debounceInterval);              // interval in ms
		Serial << "Button " << i << " setup" << endl;
	}

	//Setup LED pins
	for (int i = 0; i < NUM_BUTTONS; i++) {
		pinMode(BUTTON_LIGHT_PINS[i], OUTPUT); // set the LED pin to output
		digitalWrite(BUTTON_LIGHT_PINS[i], HIGH); // initialize the LED in high state
		Serial << "Button " << i << " light setup" << endl;
	}

}

// Function to set up all stepper motors for AccelStepper library
void setupSteppers()
{
	handDropStepper.setEnablePin(enPinHandDropStepper); // Set enable pin for little stepper motor
	handDropStepper.setMaxSpeed(maxSpeedHandDropStepper); // Set maximum speed for little stepper motor
	handDropStepper.setAcceleration(maxAccelHandDropStepper); // Set acceleration for little stepper motor
	handDropStepper.disableOutputs();
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
		// If it rose (was pressed), flag the need to toggle the LED
		if (buttons[i].rose()) { // If the button in the for loop was pressed
			Serial << "Button " << i << " pressed" << endl;
			// Toggle the LED
			digitalWrite(BUTTON_LIGHT_PINS[i], LOW); // Turn off the light for the button that was pressed
			if ((i == state) && (i == 1)) { // if the button is pressed again, drain the tank
				drainTank(); // if the button is pressed again, drain the tank
				return;
			}
		state = static_cast<State>(i); // set the state to the button that was pressed
		lastButtonPressTime = millis(); // update the last button press time for timeout
		}
	}
}

// Function to dispense X mL in the specified direction
void stepperDispense(AccelStepper stepper, long uL, bool forward, long uLsPerRev)
{
	stepper.enableOutputs(); // Enable the stepper motor outputs
	// Calculate the number of steps required to dispense the specified uL
	int steps = round((float)uL / uLsPerRev * stepsPerRev);
	Serial << "Dispensing " << uL << " uL (" << steps << " steps) " << (forward ? "forwards... " : "backwards... ") << endl;
	stepper.move(forward ? -steps : steps); // Move the stepper motor forward or backward by the specified number of steps
	// Run the stepper motor until it reaches the target position ////blocking code
  if (stepper.distanceToGo() != 0) {
    stepper.run();
  } else {
    stepper.disableOutputs(); // Call this once the movement is complete
  }

	Serial.println(" done");
}

void timeout()
{
	if (lastButtonPressTime > timeoutMillis)
	{
		Serial << "Timeout" << endl;
		drainTank();
	}
}

void dispense()
{
	state = BIG_DROP_STATE; // FOR TESTING
    switch (state)
  {
    case HAND_DROP_STATE:
      Serial << "Hand drop state" << endl;
      stepperDispense(handDropStepper, handDropVolume, true, uLsPerRevSmallStepper);
      digitalWrite(BUTTON_LIGHT_PINS[HAND_DROP], HIGH); // Turn on the light for the hand drop button after dispensing
			//state = RESET_STATE; FOR TESTING
      break;
		case BIG_DROP_STATE:
      digitalWrite(BUTTON_LIGHT_PINS[LARGE_TANK], LOW); // Turn off the light for the large tank button to show that its full
      Serial << "Big tank drop state" << endl;
      sumpPumpDispense(bigTankVolume);
			delay(5000);
      break;
		case RESET_STATE:
			return;
    default:
      Serial << "Invalid state" << endl;
      break;
	}
}

void sumpPumpDispense(int mLs)
{
	const unsigned long sumpDelay = (mLs / mLsPerSecondSumpPump * 1000);
	unsigned long currentMillis = millis();
	Serial << "Dispensing " << mLs << " mLs sump pump... " << endl;
	digitalWrite(sumpPumpRelayPin, HIGH);
	if (currentMillis - previousMillis >= sumpDelay) {
		digitalWrite(sumpPumpRelayPin, LOW);
		Serial << "Done Pumping Sump Pump" << endl;
		previousMillis = currentMillis;
	}
}

void drainTank()
{
	if (largeTankState == DISPENSING) {
		Serial << "Large tank is dispensing, cannot drain" << endl;
		return; // Do not drain if the large tank is dispensing, returns out of function
	}
	Serial << "Draining tank" << endl;
	digitalWrite(drainRelayPin, LOW); // Turn on the relay to drain the big tank. REVERSE LOGIC! (LOW = ON)
	delay(tankDrainDuration); // Wait for the specified duration, blocking
	digitalWrite(drainRelayPin, HIGH); // Turn off the relay to close the valve. REVERSE LOGIC! (HIGH = OFF)
	//overflowCheck(LARGE_TANK); // Check for overflow after draining the tank. Currently redundant with ISR, but may be useful in the future if we want a full check
	digitalWrite(BUTTON_LIGHT_PINS[LARGE_TANK], HIGH); // Turn on the light for the small tank button
	largeTankState = IDLE; // Set the large tank state to idle
	state = RESET_STATE;
	Serial << "Tank drained" << endl;
}
/*
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
*/
void resolveOverflow(int tank) {
	const int maxRetries = 5;
	const unsigned long delayDuration = 10000; // 10 seconds for retry delay

	for (int retry = 0; retry < maxRetries; ++retry) {
			drainTank();  // Attempt to drain the tank(s)
			delay(delayDuration);  // Wait before rechecking
		/*
			// Recheck for overflow condition
			if (!(tank == LARGE_TANK || tank == BOTH_TANKS) || !digitalRead(bigTankOverflowPin)) {
					if (!(tank == SMALL_TANK || tank == BOTH_TANKS) || !digitalRead(smallTankOverflowPin)) {
							Serial << "Overflow resolved after " << (retry + 1) << " attempts." << endl;
							// Check which tank it is, and set that tank to idle
							LARGE_TANK == tank ? largeTankState = IDLE : smallTankState = IDLE;
							return; // Overflow successfully resolved
					}
			}
		*/
	}
	Serial << "Overflow could not be resolved after " << maxRetries << " attempts. Stopping program." << endl;
	while(1) {
		Serial << "Program Stop due to overflow" << endl;
		delay(5000);
	}
}

void overflowLargeTankISR() {
	Serial << "Big tank overflow detected." << endl;
	largeTankState = OVERFLOW;
	resolveOverflow(LARGE_TANK);
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

void testDrain() {
	Serial << "Test Draining Tank" << endl;
	digitalWrite(drainRelayPin, HIGH); // Turn on the relay to drain the big tank
	delay(5000); // Wait for the specified duration
	digitalWrite(drainRelayPin, LOW); // Turn off the relay to close the valve
	delay(5000);
	Serial << "Test Finished" << endl;
}

void testSumpPump() {
	Serial << "Test Sump Pump" << endl;
	digitalWrite(sumpPumpRelayPin, HIGH);
	delay(5000);
	digitalWrite(sumpPumpRelayPin, LOW);
	delay(5000);
	Serial << "Done Pumping Sump Pump" << endl;
}

void testStepperDispense() {
	Serial << "Test Stepper Dispense, 5 rev" << endl;
	handDropStepper.enableOutputs(); // Enable the stepper motor outputs
	// Calculate the number of steps required to dispense the specified uL
	handDropStepper.move(stepsPerRev*100); // Move the stepper motor forward or backward by the specified number of steps
	while (handDropStepper.distanceToGo() != 0) {
			handDropStepper.run(); // Run the stepper motor until it reaches the target position
	}
	Serial << "Done Stepper Dispense" << endl;
	delay(5000);
}