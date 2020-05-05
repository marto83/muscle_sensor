#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/*
  Analog input, analog output, serial output

  Reads an analog input pin, maps the result to a range from 0 to 255 and uses
  the result to set the pulse width modulation (PWM) of an output pin.
  Also prints the results to the Serial Monitor.

  The circuit:
  - potentiometer connected to analog pin 0.
    Center pin of the potentiometer goes to the analog pin.
    side pins of the potentiometer go to +5V and ground
  - LED connected from digital pin 9 to ground

  created 29 Dec. 2008
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/AnalogInOutSerial
*/

// These constants won't change. They're used to give names to the pins used:
const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const int analogOutPin = 9; // Analog output pin that the LED is attached to

const int ledPin = 15;
const int analogPin = 21;
const int relayPin = 22;
const int musclePin = A0;

const int buttonOnePin = 11;
const int buttonTwoPin = 12;

int sensorValue = 0; 
int analogValue = 0; // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)

int modeToggleButtonState = 0;  
int buttonTwoState = 0;

struct Display {
  String line1;
  String line2;
};

struct Display DisplayContent;

enum ProgramMode { 
  measure = 0,
  playback = 1
};

enum ProgramMode appMode;

void setup() {
  appMode = playback;

  //Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, 400000);
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();

  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  
  pinMode(ledPin, OUTPUT);    
  pinMode(relayPin, OUTPUT);

  pinMode(buttonOnePin, INPUT_PULLUP);
  pinMode(buttonTwoPin, INPUT_PULLUP);

  displayCurrentMode();
 // init();
}

void displayText(String text) {
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(text);

  display.display();
}

void displayContent() {
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text

  display.setCursor(0,0);
  display.println(DisplayContent.line1);
  display.println(DisplayContent.line2);
  display.display();
}

void displayCurrentMode() {
  if(appMode == playback) {
    displayText("Playback");
  } else if (appMode == measure) 
  { 
    displayText("Measure");
  }
  else
  {
    displayText("Record");
  }
}

void toggleMode () {
  
  if( appMode == playback) {
    appMode = measure;  
  else {
    appMode = playback;
  }
  
  displayCurrentMode();
}

void loop() {
  // read the analog in value:

  modeToggleButtonState = digitalRead(buttonOnePin);
  buttonTwoState = digitalRead(buttonTwoState); 

  if (modeToggleButtonState == LOW) {
    toggleMode();
    delay(200);
  }
  
  sensorValue = analogRead(musclePin);
  int newAnalog = analogRead(analogPin);
  Serial.print("analogValue = ");
  Serial.println(newAnalog);
  if(newAnalog != analogValue)
  {
    analogValue = newAnalog;
    String value = "value: ";
    String toPrint = value + analogValue;
    //displayText(toPrint);
  }
  // map it to the range of the analog out:
 // outputValue = map(sensorValue, 0, 1023, 0, 255);
  // change the analog out value:
//  analogWrite(analogOutPin, outputValue);

  // print the results to the Serial Monitor:
  Serial.print("sensor = ");
  Serial.println(sensorValue);
//  Serial.print("\t output = ");
//  Serial.println(outputValue);
  if ( analogValue > 300)
  {
    digitalWrite(ledPin, HIGH);
    digitalWrite(relayPin, HIGH);
  // wait 2 milliseconds before the next loop for the analog-to-digital
  // converter to settle after the last reading:
    delay(20);
  }
  else 
  {
    digitalWrite(relayPin, LOW);

    digitalWrite(ledPin, LOW); 
  }
}
