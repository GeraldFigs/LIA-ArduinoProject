#include <LiquidCrystal.h>  //include the LiquidCrystal library for LCD control

// Sensor, actuator, and other associated variables
int sensor = A1;  // Analog input pin for sensor is A1.
int actuator = 10;  // Actuator connected to digital pin 10.
int PV = 0;  // Process Variable, initialized as 0.
int setPoint = 0;  // Desired value (setPoint) for process variable, initialized as 0.
int CO = 0;  // Control Output, initialized as 0.
int HYS = 30;  // Hysteresis value, initialized as 30.

// Button pins definitions.
const int buttonPinLeft = 6;  
const int buttonPinRight = 7; 
const int buttonPinUp = 8;
const int buttonPinDown = 9;

// Button states and debounce variables
bool butonStateLeft, lastbutonStateLeft, buttonStateRight, lastbuttonStateRight, buttonStateUp, lastbuttonStateUp, buttonStateDown, lastbuttonStateDown = false;
unsigned long lastButtonPressLeft, lastButtonPressRight, lastButtonPressUp, lastButtonPress4 = 0;
const unsigned long debounceDelay = 50;

// LCD page variable
int currentPage = 0;  // Current page number on the LCD screen, initialized as 0.

// Define the LCD connections
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;  // Pins connected to the LCD.
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  Serial.begin(9600);  // Start the serial communication with baud rate 9600.
  lcd.begin(16, 2);  // Initialize the LCD and specify the dimensions (16x2).

  // INPUTS
  pinMode(buttonPinLeft, INPUT_PULLUP);  // Set the buttonPinLeft as input with internal pull-up resistor.
  pinMode(buttonPinRight, INPUT_PULLUP);  // Set the buttonPinRight as input with internal pull-up resistor.
  pinMode(buttonPinUp, INPUT_PULLUP);  // Set the buttonPinUp as input with internal pull-up resistor.
  pinMode(buttonPinDown, INPUT_PULLUP);  // Set the buttonPinDown as input with internal pull-up resistor.
  // OUTPUTS
  pinMode(actuator, OUTPUT);  // Set the actuator pin as output.
}

void loop() {
  PV = analogRead(sensor);  // Read the sensor value through the analog pin.

  // Implement the hysteresis logic.
  if (PV <= (setPoint - HYS)) {  // If process variable is less than the lower hysteresis limit,
    CO = 255;  // set the control output to 255.
  } else if (PV >= (setPoint + HYS)) {  // Else if process variable is more than the upper hysteresis limit,
    CO = 0;  // set the control output to 0.
  }
  analogWrite(actuator, CO);  // Write the control output to the actuator.

/*Serial.print("SP = ");
Serial.print(setPoint);
Serial.print("\tCO = ");
Serial.print(CO);
Serial.print("\tPV = ");
Serial.print(PV);
Serial.print("\tHYS = ");
Serial.print(HYS);
Serial.print("\tPage = ");
Serial.println(currentPage);*/

// Send data to the Serial Plotter
  Serial.print(setPoint);     //BLUE
  Serial.print(",");
  Serial.print(CO);           //DARK ORANGE
  Serial.print(",");
  Serial.print(PV);           //GREEN
  Serial.print(",");
  Serial.println(HYS);        //LT ORANGE

  // Debounce and button actions
  butonStateLeft = digitalRead(buttonPinLeft);  // Read the state of the left button.
  buttonStateRight = digitalRead(buttonPinRight);  // Read the state of the right button.
  buttonStateUp = digitalRead(buttonPinUp);  // Read the state of the up button.
  buttonStateDown = digitalRead(buttonPinDown);  // Read the state of the down button.

  // Check if left button is pressed, and debounced.
  if (!butonStateLeft && lastbutonStateLeft && (millis() - lastButtonPressLeft) > debounceDelay) {
    currentPage--;  // Decrement the current page.
    if (currentPage < 0) currentPage = 3;  // If current page is less than 0, set it to 3.
    lastButtonPressLeft = millis();  // Update the last button press time for left button.
  }
  lastbutonStateLeft = butonStateLeft;  // Store the state of left button for debouncing.

  // Check if right button is pressed, and debounced.
  if (!buttonStateRight && lastbuttonStateRight && (millis() - lastButtonPressRight) > debounceDelay) {
    currentPage++;  // Increment the current page.
    if (currentPage > 3) currentPage = 0;  // If current page is more than 3, set it to 0.
    lastButtonPressRight = millis();  // Update the last button press time for right button.
  }
  lastbuttonStateRight = buttonStateRight;  // Store the state of right button for debouncing.

  // Check if up button is pressed, and debounced.
  if (!buttonStateUp && (millis() - lastButtonPressUp) > debounceDelay) {
    if (currentPage == 0) {  // If current page is 0,
      setPoint += 5;  // increment setPoint by 5.
      if (setPoint < 0) {  // If setPoint is less than 0,
        setPoint = 0;  // reset it to 0.
        lcd.setCursor(0, 1);
        lcd.print("SP can't be < 0!");  // Print error message on LCD.
        delay(1000);  // Wait for 1 second.
      }
    } else if (currentPage == 3) {  // Else if current page is 3,
      HYS += 5;  // increment HYS by 5.
      if (HYS < 0) {  // If hysteresis is less than 0,
        HYS = 0;  // Reset it to 0.
        lcd.setCursor(0, 1);  // Set LCD cursor position.
        lcd.print("HYS can't be < 0!");  // Print error message on LCD.
        delay(1000);  // Wait for 1 second.
      }
    }
    lastButtonPressUp = millis();  // Update the last button press time for up button.
  }
  lastbuttonStateUp = buttonStateUp;  // Store the state of up button for debouncing.

  // Check if down button is pressed, and debounced.
  if (!buttonStateDown && (millis() - lastButtonPress4) > debounceDelay) {
    if (currentPage == 0) {  // If current page is 0,
      setPoint -= 5;  // Decrement setPoint by 5.
      if (setPoint < 0) {  // If setPoint is less than 0,
        setPoint = 0;  // Reset it to 0.
        lcd.setCursor(0, 1);  // Set LCD cursor position.
        lcd.print("SP CAN'T BE <0!");  // Print error message on LCD.
        delay(1000);  // Wait for 1 second.
      }
    } else if (currentPage == 3) {  // Else if current page is 3,
      HYS -= 5;  // Decrement HYS by 5.
      if (HYS < 0) {  // If hysteresis is less than 0,
        HYS = 0;  // Reset it to 0.
        lcd.setCursor(0, 1);  // Set LCD cursor position.
        lcd.print("HYS CAN'T BE <0!");  // Print error message on LCD.
        delay(1000);  // Wait for 1 second.
      }
    }
    lastButtonPress4 = millis();  // Update the last button press time for down button.
  }
  lastbuttonStateDown = buttonStateDown;  // Store the state of down button for debouncing.

  // Update the LCD display based on currentPage
 lcd.clear();  // Clear the LCD screen.
  switch (currentPage) {  // Depending on the current page,
    case 0:  // If page 0,
      lcd.setCursor(0, 0);  // Set cursor position.
      lcd.print("SP:");  // Print "SP:".
      lcd.print(setPoint);  // Print the current set point.
      lcd.setCursor(0, 1);  // Set cursor position for next line.
      break;
    case 1:  // If page 1,
      lcd.setCursor(0, 0);  // Set cursor position.
      lcd.print("PV:");  // Print "PV:".
      lcd.print(PV);  // Print the current process variable.
      break;
    case 2:  // If page 2,
      lcd.setCursor(0, 0);  // Set cursor position.
      lcd.print("CO:");  // Print "CO:".
      lcd.print(CO);  // Print the current control output.
      break;
    case 3:  // If page 3,
      lcd.setCursor(0, 0);  // Set cursor position.
      lcd.print("HYS:");  // Print "HYS:".
      lcd.print(HYS);  // Print the current hysteresis.
      lcd.setCursor(0, 1); // Set cursor position for next line.
      break;
  }
  // Delay to avoid screen flicker
  delay(100);  // Wait for 100 milliseconds.
}