#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Streaming.h> // Library for printing in streaming method
#include <Bounce2.h>   // Library for debouncing button

#define OLED_RESET -1		// Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Bounce button = Bounce();
const int BUTTON_PIN = 11;
void pollSerial()
{
	// if serrial command F is received, and trank is empty, fill tank
	// if serial command D is received, and tank is full, give a drop
	if (Serial.available() > 0)
	{
		char inChar = Serial.read();
		if (inChar == 'F') //&& largeTankState == EMPTY)
		{
			// Serial << "Filling tank" << endl;
			display.clearDisplay();
			display.setCursor(0, 0); // Start at top-left corner
			display.println(F("Filling tank"));
			display.display();

			// sumpPumpDispense();
		}
		else if (inChar == 'D')
		{ // && largeTankState == FULL) {
			// Serial << "Dispensing hand drop" << endl;
			// stepperDispense();
			display.clearDisplay();
			display.setCursor(0, 0); // Start at top-left corner
			display.println(F("Dispensing hand drop"));
			display.display();
		}
		else if (inChar == 'R')
		{
			display.clearDisplay();
			display.setCursor(0, 0); // Start at top-left corner
			display.println(F("Drained, Awaiting User Action"));
			display.display();
		}
	}
}

void setup()
{
	Serial.begin(9600);
	setupButtons();
	// SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
	if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
	{
		Serial.println(F("SSD1306 allocation failed"));
		for (;;)
			; // Don't proceed, loop forever
	}
	display.clearDisplay();

	display.setTextSize(1);				 // Normal 1:1 pixel scale
	display.setTextColor(SSD1306_WHITE); // Draw white text
	display.setCursor(0, 0);			 // Start at top-left corner
	display.println(F("Hello, world!"));

	display.display();
}
bool ledState = false;
void loop()
{
	pollSerial();
	button.update(); // Update the Bounce instance :
	if (button.fell())
	{
		// Serial << "Button pressed" << endl;
		Serial.println('X');
		digitalWrite(10, ledState);
		ledState = !ledState;
		display.clearDisplay();
		display.setCursor(0, 0); // Start at top-left corner
		display.println(F("Litte Drip Dripped"));
		display.display();
	}
}

void setupButtons()
{
	button.attach(BUTTON_PIN, INPUT_PULLUP); // setup the bounce instance for the current button
	button.interval(30);					 // interval in ms
	pinMode(10, OUTPUT);
}
