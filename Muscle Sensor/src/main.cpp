#include <Arduino.h>
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

const int knobPin = 21;
const int muscle1Pin = A0;
const int muscle1LedPin = 16;
const int muscle2Pin = A1;
const int muscle2LedPin = 17;

const int switch1Pin = 6;
const int switch2Pin = 7;

const int modeButtonPin = 10;
const int selectButtonPin = 11;
const int confirmButtonPin = 12;

unsigned long time;
int muscle1Value = 0;
bool switch1Pressed = false;
bool switch2Pressed = false;
int muscle2Value = 0;
int knobValue = 0; // value read from the pot

int modeButtonPinState = 0;
int selectButtonPinState = 0;
int confirmButtonState = 0;

struct Display {
  String line1;
  String line2;
  String line3;
};

struct Display DisplayContent;

struct MeasureData {
  int muscle1;
  int muscle2;
  int selectedMuscle;
  int muscle1Threshold = 0;
  int muscle2Threshold = 0;
  int knobValue;
} measureData;

enum ProgramMode {
  measure = 0,
  playback = 1
};

enum ProgramMode appMode;

void getButtonStates() {
  modeButtonPinState = digitalRead(modeButtonPin);
  selectButtonPinState = digitalRead(selectButtonPin);
  confirmButtonState = digitalRead(confirmButtonPin);
}

void readAnalogValues() {
  muscle1Value = analogRead(muscle1Pin);
  muscle2Value = analogRead(muscle2Pin);
  knobValue = analogRead(knobPin);
}

bool shouldRenderMeasureData(){
  return measureData.knobValue != knobValue
  || measureData.muscle1 != muscle1Value
  || measureData.muscle2 != muscle2Value;
}

void DisplayMeasureContent()
{
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text

  
   String line1 = "M1: ";
   line1 += measureData.muscle1;
   if(measureData.selectedMuscle == 1)
    line1 += " < ";
   String line2 = "M2: "; 
   line2 += measureData.muscle2;
   if(measureData.selectedMuscle == 2)
    line2 += " <";
   line1 = line1 + "   T: ";
   line1 += measureData.muscle1Threshold;
   line2 = line2 + "   T: ";
   line2 += measureData.muscle2Threshold;
   String line3 = "Knob value: ";
   line3 += measureData.knobValue;
    display.setCursor(0,0);
    display.println(line1);
    display.println(line2);
    display.println(line3);
    display.display();
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
  display.println(DisplayContent.line3);
  display.display();

  // Serial.println(content.line1);
  // Serial.println(content.line2);
  // Serial.println(content.line3);
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
  delay(100);
}

void handleLeds()
{
  if(muscle1Value >= measureData.muscle1Threshold)
  {
     digitalWrite(muscle1LedPin, HIGH);
  } else {
    digitalWrite(muscle1LedPin, LOW);
  }

  if(muscle2Value >= measureData.muscle2Threshold)
  {
    digitalWrite(muscle2LedPin, HIGH);
  } else {
    digitalWrite(muscle2LedPin, LOW);
  }
}

void handleSwitches()
{
  if(appMode == playback)
  {
    if(muscle1Value >= measureData.muscle1Threshold)
    {
      if(switch1Pressed == false)
      {
        digitalWrite(switch1Pin, HIGH);
        delay(100);
        digitalWrite(switch1Pin, LOW);
        displayText("Button1 pressed.");
        switch1Pressed = true;
      }
    }
     else if(switch1Pressed) {
      displayText("Button1 cleared.");
      switch1Pressed = false;
    }

    if(muscle2Value >= measureData.muscle2Threshold)
    {
      if(switch2Pressed == false)
      {
        digitalWrite(switch2Pin, HIGH);
        delay(100);
        digitalWrite(switch2Pin, LOW);
        displayText("Button2 pressed.");
        switch2Pressed = true;
      } 
    }
    else if(switch2Pressed) {
        displayText("Button2 cleared.");
        switch2Pressed = false;
      }
  }
}

void toggleMode () {

  if( appMode == playback) {
    appMode = measure;
  }
  else {
    switch2Pressed = false;
    switch1Pressed = false;
    appMode = playback;
  }

  // TODO: If mode is now playback. Record thresholds in EEPROM
  displayCurrentMode();
}

void setMode()
{
  if (modeButtonPinState == LOW) {
    toggleMode();
    delay(200);
  }
}

void setup() {
  appMode = playback;
  measureData.selectedMuscle =0;
  //measureData.muscle1Threshold = //Read EEPROM;
  //measureData.muscle2Threshold = //Read EEPROM;

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

  pinMode(muscle1LedPin, OUTPUT);
  pinMode(muscle2LedPin, OUTPUT);
  pinMode(switch1Pin, OUTPUT);
  pinMode(switch2Pin, OUTPUT);

  pinMode(modeButtonPin, INPUT_PULLUP);
  pinMode(selectButtonPin, INPUT_PULLUP);
  pinMode(confirmButtonPin, INPUT_PULLUP);

  displayCurrentMode();
 // init();
}

void loop() {
  // read the analog in value:

  getButtonStates();
  setMode();
  readAnalogValues();

  if(appMode == measure)
  {
      bool shouldRerender = shouldRenderMeasureData();
      measureData.muscle1 = muscle1Value;
      measureData.muscle2 = muscle2Value;
      measureData.knobValue = knobValue;

      if(selectButtonPinState == LOW)
      {
        measureData.selectedMuscle++;
        if(measureData.selectedMuscle > 2)
          measureData.selectedMuscle = 0;
        delay(500);
      }

      if(confirmButtonState == LOW)
      {
        if(measureData.selectedMuscle == 1)
        {
          measureData.muscle1Threshold = knobValue;
        } else if (measureData.selectedMuscle == 2)
        {
          measureData.muscle2Threshold = knobValue;
        }
        delay(200);
      }
      // String test = "Test: ";
      // test += measureData.knobValue;
      // displayText(test);
      if(shouldRerender)
        DisplayMeasureContent();
  }
  else {

  }

  handleLeds();
  handleSwitches();
}