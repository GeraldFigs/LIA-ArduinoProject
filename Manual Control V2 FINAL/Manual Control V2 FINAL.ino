#include <LiquidCrystal.h> // Include the LiquidCrystal library for controlling the LCD

// Define pins for the up and down buttons
const int buttonPinUp = 8;    
const int buttonPinDown = 9; 

// Initialize button states and debounce variables
bool buttonStateUp, lastbuttonStateUp, buttonStateDown, lastbuttonStateDown = false;  // Current and previous states of both buttons
// Variables for button states (last and current): initially all false (unpressed)
unsigned long lastButtonPressUp, lastButtonPressDown = 0; // Time of the last button press for both buttons
// Variables for storing the last button press times: initially 0
const unsigned long debounceDelay = 50; // Define debounce delay as 50 ms to avoid multiple triggers of the buttons

// Define LCD pin connections
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2; // These are the LCD interface pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);  // Initialize the LCD object with the pin connections

// Define variables for the sensor pin, actuator pin, process value, input value, and control output
int sensor = A1;
//int inputPin = A5; // Use this input when using potentiometer as the SP
int actuator = 10; // The pin for the actuator is 10
int PV = 0; // Process Variable initially is set to 0
int inputValue = 0; // inputValue initially is set to 0
int CO = 0; // Control Output initially is set to 0

void setup() {
  pinMode(actuator, OUTPUT);  // Set the actuator pin as output
  Serial.begin(9600);  // Begin serial communication at 9600 baud rate for debugging

  lcd.begin(16, 2); // Start the LCD at 16 cols and 2 rows

  // Set the button pins as input pull-ups
  pinMode(buttonPinUp, INPUT_PULLUP);
  pinMode(buttonPinDown, INPUT_PULLUP);
}

void loop() {
  PV = analogRead(sensor);  // Read the sensor value
  //inputValue = analogRead(inputPin);  // Use this input when using potentiometer as the SP
  CO = map(inputValue, 0, 1023, 0, 255);  // Map the input value to a range suitable for the actuator
  analogWrite(actuator, CO);  // Write the mapped value to the actuator

  // Print debugging information to the serial monitor
  // These lines print out the input, CO and PV values to the serial monitor
  Serial.print("input = ");
  Serial.print(inputValue);
  Serial.print("\t CO = ");
  Serial.print(CO);
  Serial.print("\t PV = ");
  Serial.print(PV);
  Serial.println();

  // Read the state of the up and down buttons
 buttonStateUp = digitalRead(buttonPinUp);  // Reads the state of the up button
  buttonStateDown = digitalRead(buttonPinDown);  // Reads the state of the down button



  if (!buttonStateUp && (millis() - lastButtonPressUp) > debounceDelay) { // If the up button is pressed and debounced
    inputValue += 5; // increment the inputValue by 5
    if (inputValue < 0) { // if inputValue goes below 0
      inputValue = 0; // reset it back to 0
      lcd.setCursor(0, 1); // set the cursor of the LCD to column 0, row 1
      lcd.print("SP can't be < 0!"); // print the error message to the LCD
      delay(1000); // delay for 1 second
    } else if (inputValue > 1024) { // if inputValue goes above 1024
      inputValue = 1020; // set it back to 1020
      lcd.setCursor(0, 1); // set the cursor of the LCD to column 0, row 1
      lcd.print("SP CAN'T BE>1024!"); // print the error message to the LCD
      delay(1000); // delay for 1 second
    }
    lastButtonPressUp = millis(); // save the current time as the last button press time
  }
  lastbuttonStateUp = buttonStateUp; // save the current button state as the last button state



  if (!buttonStateDown && (millis() - lastButtonPressDown) > debounceDelay) { // If the down button is pressed and debounced
    inputValue -= 5; // decrement the inputValue by 5
    if (inputValue < 0) { // if inputValue goes below 0
      inputValue = 0; // reset it back to 0
      lcd.setCursor(0, 1); // set the cursor of the LCD to column 0, row 1
      lcd.print("SP CAN'T BE <0!"); // print the error message to the LCD
      delay(1000); // delay for 1 second
    } else if (inputValue > 1024) { // if inputValue goes above 1024
      inputValue = 1020; // set it back to 1020
      lcd.setCursor(0, 1); // set the cursor of the LCD to column 0, row 1
      lcd.print("SP CAN'T BE>1024!"); // print the error message to the LCD
      delay(1000); // delay for 1 second
    }
    lastButtonPressDown = millis(); // save the current time as the last button press time
  }
  lastbuttonStateDown = buttonStateDown; // save the current button state as the last button state

  lcd.clear(); // clears the LCD screen
  // Display inputValue on the first row of the LCD
  lcd.setCursor(0, 0); // set the cursor of the LCD to column 0, row 0
  lcd.print("inputVal:"); // prints "inputVal:" to the LCD
  lcd.print(inputValue); // prints the current inputValue to the LCD

  // Display PV on the second row of the LCD
  lcd.setCursor(0, 1); // set the cursor of the LCD to column 0, row 1
  lcd.print("PV:"); // prints "PV:" to the LCD
  lcd.print(PV); // prints the current PV value to the LCD


  // Display CO on the second row of the LCD after "PV"
  lcd.setCursor(8, 1); // set the cursor of the LCD to column 8, row 1
  lcd.print("CO:"); // prints "CO:" to the LCD
  lcd.print(CO); // prints the current CO value to the LCD
}