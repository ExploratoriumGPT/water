# Scale of Water Arduino

This is an Arduino project that controls a water dispensing system. It uses stepper motors to dispense water in precise quantities. The system is designed to prevent flooding and has a timeout feature to drain the tanks if necessary.

## Features
- Uses the AccelStepper library to control stepper motors for precise water dispensing.
- Uses the Bounce2 library for debouncing button inputs.
- EEPROM is used to store the state of the system.
- The system can be calibrated to dispense specific volumes of water.
- The system has a timeout feature that drains the tanks if they are not used for a certain period of time.
- The system has a feature to prevent flooding by adding a water level sensor (to be implemented).

## Hardware Requirements
- Arduino Mega
- Stepper motors
- Pushbuttons
- Relay
- EEPROM
- OLED screen (to be implemented)

## Software Requirements
- Arduino IDE
- AccelStepper library
- Bounce2 library

## Future Improvements
- Add stepper drain code and peristaltic hardware for medium tank drain.
- Add code for lit pushbuttons, lite the button one is allowed to push only.
- Refactor code for ray stack and Arduino Mega.
- Write drain on boot code.
- Print to OLED screen instead of or in addition to the Serial.
- Remove EEPROM code once boot drain is implemented.
- Consider adding water level sensor to prevent flooding if drain issue on main tank.

## How to Use
1. Connect the hardware as per the pin configuration in the code.
2. Upload the code to the Arduino.
3. Press the button to start the water dispensing routine. The routine will go through several states, dispensing different amounts of water in each state.
4. If the button is not pressed for a certain period of time, the system will automatically drain the tanks.
