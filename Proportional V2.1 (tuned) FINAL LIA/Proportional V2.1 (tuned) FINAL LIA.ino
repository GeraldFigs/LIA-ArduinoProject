#include <LiquidCrystal.h>  //include the LiquidCrystal library for LCD control

// Define the pin connected to the sensor.
int sensor = A1;
//int inputPin = A5;
int actuator = 10;  // Define the actuator pin. This pin controls the output (actuator).
int PV = 0; // Current value of the variable we want to control, 
int setPoint = 0; // The desired value of the variable we want to control,
int CO = 0; // The value we will set on the output to the actuator, 
int error = 0;  // The difference between the Set Point and the Process Variable.
float pGain = 0.3;  // Define the proportional gain. This value is used in the proportional control equation.

// Pushbuttons
const int buttonPinLeft = 6;
const int buttonPinRight = 7;
const int buttonPinUp = 8;
const int buttonPinDown = 9;

// Define variables to hold the current and previous states of each button. This is used for debouncing the buttons.
bool buttonStateLeft, lastButtonStateLeft, buttonStateRight, lastButtonStateRight, buttonStateUp, lastButtonStateUp, buttonStateDown, lastButtonStateDown = false;

// Define variables to store the time of the last button press. This is also used for debouncing.
unsigned long lastButtonPressLeft, lastButtonPressRight, lastButtonPressUp, lastButtonPressDown = 0;

// Define the debounce delay in milliseconds.
const unsigned long debounceDelay = 50;

// Define a variable to store the current page displayed on the LCD.
int currentPage = 0;

// LCD pin connections
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2); // Initialize the LCD and define the number of columns and rows it has.
  pinMode(actuator, OUTPUT);  // Set the actuator pin as output.
  Serial.begin(9600); // Start the serial communication with the computer.

  // Set the button pins as inputs with internal pull-up resistors. 
  pinMode(buttonPinLeft, INPUT_PULLUP);
  pinMode(buttonPinRight, INPUT_PULLUP);
  pinMode(buttonPinUp, INPUT_PULLUP);
  pinMode(buttonPinDown, INPUT_PULLUP);
}

void loop() {
  PV = analogRead(sensor); // Read the sensor value.
  error = setPoint-PV;  // Calculate the error as the difference between the set point and the process variable.
  CO = (pGain*error) / 10;    // Calculate the control output. This is the proportional control equation.

  // Limit the control output to the range of 0-255. This is because analogWrite only accepts values in this range.
  if (CO >255) {
    CO = 255;
  }
  if (CO < 0) {
    CO = 0;
  }
/*
Serial.print("SP = ");
Serial.print(setPoint);
Serial.print("\t CO = ");
Serial.print(CO);
Serial.print("\t     PV = ");
Serial.print(PV);
Serial.print("\tp Gain = ");
Serial.print(pGain);
Serial.println(); */

// Send the set point, control output, process variable and error values to the serial port.
  Serial.print(setPoint);     //BLUE
  Serial.print(",");
  Serial.print(CO);           //DARK ORANGE
  Serial.print(",");
  Serial.print(PV);           //GREEN
  Serial.print(",");
  Serial.println(error);        //LT ORANGE*/

  // Read the current state of the buttons.
  buttonStateLeft = digitalRead(buttonPinLeft);
  buttonStateRight = digitalRead(buttonPinRight);
  buttonStateUp = digitalRead(buttonPinUp);
  buttonStateDown = digitalRead(buttonPinDown);

  // Button actions with debouncing. If a button is pressed (input is LOW because of pullup resistor) and previously it was not pressed, then we change the state.
  // Left button decreases the currentPage value, moves to the previous page.
  if (!buttonStateLeft && lastButtonStateLeft && (millis() - lastButtonPressLeft) > debounceDelay) {
    currentPage--;
    if (currentPage < 0) currentPage = 3; // Keep the currentPage value within the limit
    lastButtonPressLeft = millis();
  }
  lastButtonStateLeft = buttonStateLeft;

  // Right button increases the currentPage value, moves to the next page.
  if (!buttonStateRight && lastButtonStateRight && (millis() - lastButtonPressRight) > debounceDelay) {
    currentPage++;
    if (currentPage > 3) currentPage = 0; // Keep the currentPage value within the limit
    lastButtonPressRight = millis();
  }
  lastButtonStateRight = buttonStateRight;

  // The Up button increases the value of setPoint or pGain depending on the current page.
  if (!buttonStateUp && lastButtonStateUp && (millis() - lastButtonPressUp) > debounceDelay) {
    if (currentPage == 0) {
      setPoint += 5; // Increment setPoint if on page 0
      if (setPoint < 0) { // Check to ensure setPoint is not less than 0
        setPoint = 0;
        lcd.setCursor(0, 1);
        lcd.print("SP can't be < 0!");
        delay(1000);
      }
    } else if (currentPage == 3) {
      pGain += 0.01; // Increment pGain if on page 3
      if (pGain < 0) { // Check to ensure pGain is not less than 0
        pGain = 0;
        lcd.setCursor(0, 1);
        lcd.print("PROPORTIONAL can't be < 0!");
        delay(1000);
      }
    }
    lastButtonPressUp = millis();
  }
  lastButtonStateUp = buttonStateUp;


  // The Down button decreases the value of setPoint or pGain depending on the current page.
  if (!buttonStateDown && lastButtonStateDown && (millis() - lastButtonPressDown) > debounceDelay) {
    if (currentPage == 0) {
      setPoint -= 5; // Decrement setPoint if on page 0
      if (setPoint < 0) { // Check to ensure setPoint is not less than 0
        setPoint = 0;
        lcd.setCursor(0, 1);
        lcd.print("SP CAN'T BE <0!");
        delay(1000);
      }
    } else if (currentPage == 3) {
      pGain -= 0.01; // Decrement pGain if on page 3
      if (pGain < 0) { // Check to ensure pGain is not less than 0
        pGain = 0;
        lcd.setCursor(0, 1);
        lcd.print("PROPORTIONAL CAN'T BE <0!");
        delay(1000);
      }
    }
    lastButtonPressDown = millis();
  }
  lastButtonStateDown = buttonStateDown;

  // Switch between the different LCD screens (pages) based on the currentPage value.
  switch (currentPage) {
    case 0:
      lcd.setCursor(0, 0);
      lcd.print("SP:");
      lcd.print(setPoint); // Display setPoint on the first page
      lcd.setCursor(0, 1);
      break;
    case 1:
      lcd.setCursor(0, 0);
      lcd.print("PV:");
      lcd.print(PV); // Display Process Variable on the second page
      break;
    case 2:
      lcd.setCursor(0, 0);
      lcd.print("CO:");
      lcd.print(CO); // Display Control Output on the third page
      break;
    case 3:
      lcd.setCursor(0, 0);
      lcd.print("KP:");
      lcd.print(pGain); // Display proportional gain on the fourth page
      lcd.setCursor(0, 1);
      break;
  }
  // Regardless of the page, print the current error at the second row
  lcd.setCursor(0, 1);
  lcd.print("ERR:");
  lcd.print(error);
  
  // Delay to avoid screen flicker
  delay(100);
}