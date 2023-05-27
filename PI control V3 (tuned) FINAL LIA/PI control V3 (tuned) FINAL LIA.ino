#include <LiquidCrystal.h>  //include the LiquidCrystal library for LCD control

// Sensor and actuator pins
int sensor = A1; // assign analog pin A1 for the sensor
int actuator = 10;  

// Variables for storing sensor readings, control output and set points.
int PV = 0; // Process Variable, current reading from the LDR sensor.
int setPoint = 0; // Desired sensor value.
int CO = 0; // Control output, the output of the PID controller.
int error = 0; // Difference between setPoint and PV.

// Proportional and Integral gains for the PID controller.
float pGain = 0.01; // Proportional gain.
float iGain = 0.05; // Integral gain.

int integralAction; // Variable to hold the value of integral part of PID controller.
unsigned long previousMillis = 0; // Time at the previous check.
const long interval = 100; // Minimum time between PID calculations in milliseconds.
unsigned long currentMillis = 0; // Current time in milliseconds.

// Pin numbers for push buttons.
const int buttonPinLeft = 6;
const int buttonPinRight = 7;
const int buttonPinUp = 8;
const int buttonPinDown = 9;

// Variables to store the state of buttons and the time they were last pressed.
bool buttonStateLeft, lastButtonStateLeft, buttonStateRight, lastButtonStateRight, buttonStateUp, lastButtonStateUp, buttonStateDown, lastButtonStateDown = false;
unsigned long lastButtonPressLeft, lastButtonPressRight, lastButtonPressUp, lastButtonPressDown = 0;
const unsigned long debounceDelay = 50; // Delay to filter out button bounce in milliseconds.

// LCD page variable
int currentPage = 0;

// Pin numbers for LCD connection.
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); // Create an LCD object.

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2); // Initialize the 16x2 LCD.
  pinMode(actuator, OUTPUT); // Set the actuator pin as output.
  Serial.begin(9600); // Begin serial communication at 9600 bps.

  // Set all button pins as inputs with internal pull-up resistors.
  pinMode(buttonPinLeft, INPUT_PULLUP);
  pinMode(buttonPinRight, INPUT_PULLUP);
  pinMode(buttonPinUp, INPUT_PULLUP);
  pinMode(buttonPinDown, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:

  // Read the sensor value and store it in PV.
  PV = analogRead(sensor);

  // Get the current time.
  currentMillis = millis();

  // Perform PID calculations at regular intervals.
  if (currentMillis - previousMillis >= interval){
    previousMillis = currentMillis; // Update the time of last PID calculation.
    error = setPoint-PV; // Calculate the error.
    integralAction = (integralAction + error); // Calculate integral of the error.
    CO = ((pGain * error) /10)+ (iGain * integralAction); // Calculate control output.
  }
  
  // Limit the control output to 0-255.
  if (CO >255) {
    CO = 255;
  }
  if (CO < 0) {
    CO = 0;
  }
  // Ensure the error is non-negative
  if (error < 0){
    error = 0;
  }
  // Write the control output to the actuator
  analogWrite(actuator, CO);

/*Serial.print("SP = ");
Serial.print(setPoint);
Serial.print("\t CO = ");
Serial.print(CO);
Serial.print("\t     PV = ");
Serial.print(PV);
Serial.print("\tp Gain = ");
Serial.print(pGain);
Serial.print("\ti Gain = ");
Serial.print(iGain);
Serial.println();*/

  // Send setPoint, CO, PV, and pGain data to the serial plotter
  Serial.print(setPoint);     //BLUE
  Serial.print(",");
  Serial.print(CO);           //DARK ORANGE
  Serial.print(",");
  Serial.print(PV);           //GREEN
  Serial.print(",");
  Serial.println(pGain);        //LT ORANGE*/

  // Read button states and store them
  buttonStateLeft = digitalRead(buttonPinLeft);
  buttonStateRight = digitalRead(buttonPinRight);
  buttonStateUp = digitalRead(buttonPinUp);
  buttonStateDown = digitalRead(buttonPinDown);

  // Implement debounce logic and page control for the left button
  if (!buttonStateLeft && lastButtonStateLeft && (millis() - lastButtonPressLeft) > debounceDelay) {
    currentPage--;
    if (currentPage < 0) currentPage = 4;
    lastButtonPressLeft = millis();
  }
  lastButtonStateLeft = buttonStateLeft;

  // Implement debounce logic and page control for the right button
  if (!buttonStateRight && lastButtonStateRight && (millis() - lastButtonPressRight) > debounceDelay) {
    currentPage++;
    if (currentPage > 4) currentPage = 0;
    lastButtonPressRight = millis();
  }
  lastButtonStateRight = buttonStateRight;

  // Implement debounce logic and adjust setpoint or gains for the up button
  if (!buttonStateUp && (millis() - lastButtonPressUp) > debounceDelay) {
    if (currentPage == 0) {
      setPoint += 5; // Increase the setPoint when up button is pressed in page 0.
      // Display a message on the LCD if setPoint goes below 0.
      if (setPoint < 0) {
        setPoint = 0;
        lcd.setCursor(0, 1);
        lcd.print("SP can't be < 0!");
        delay(1000);
      }
    } else if (currentPage == 3) {
      pGain += 0.01; // Increase the pGain when up button is pressed in page 3.
      // Display a message on the LCD if pGain goes below 0.
      if (pGain < 0) {
        pGain = 0;
        lcd.setCursor(0, 1);
        lcd.print("PROPORTIONAL can't be < 0!");
        delay(1000);
      }
    } else if (currentPage == 4) {
      iGain += 0.01; // Increase the iGain when up button is pressed in page 4.
      // Display a message on the LCD if iGain goes below 0.
      if (iGain < 0) {
        iGain = 0;
        lcd.setCursor(0, 1);
        lcd.print("INTEGRAL CAN'T BE <0!");
        delay(1000);
      }
    }
    lastButtonPressUp = millis();
  }
  lastButtonStateUp = buttonStateUp;

  // Implement debounce logic and adjust setpoint or gains for the down button
  if (!buttonStateDown && (millis() - lastButtonPressDown) > debounceDelay) {
    if (currentPage == 0) {
      setPoint -= 5; // Decrease the setPoint when down button is pressed in page 0.
      // Display a message on the LCD if setPoint goes below 0
      if (setPoint < 0) {
        setPoint = 0;
        lcd.setCursor(0, 1);
        lcd.print("SP CAN'T BE <0!");
        delay(1000);
      }
    } else if (currentPage == 3) {
      pGain -= 0.01; // Decrease the pGain when down button is pressed in page 3.
      // Display a message on the LCD if pGain goes below 0.
      if (pGain < 0) {
        pGain = 0;
        lcd.setCursor(0, 1);
        lcd.print("PROPORTIONAL CAN'T BE <0!");
        delay(1000);
      }
    } else if (currentPage == 4) {
      iGain -= 0.01; // Decrease the iGain when down button is pressed in page 4.
      // Display a message on the LCD if iGain goes below 0.
      if (iGain < 0) {
        iGain = 0;
        lcd.setCursor(0, 1);
        lcd.print("INTEGRAL CAN'T BE <0!");
        delay(1000);
      }
    }
    lastButtonPressDown = millis();
  }
  lastButtonStateDown = buttonStateDown;

  // Clear the LCD screen and print new data based on the current page.
  lcd.clear();
  switch (currentPage) {
    case 0: // Page 0 displays the setPoint.
      lcd.setCursor(0, 0);
      lcd.print("SP:");
      lcd.print(setPoint);
      lcd.setCursor(0, 1);
      break;
    case 1: // Page 1 displays the PV.
      lcd.setCursor(0, 0);
      lcd.print("PV:");
      lcd.print(PV);
      break;
    case 2: // Page 2 displays the CO.
      lcd.setCursor(0, 0);
      lcd.print("CO:");
      lcd.print(CO);
      break;
    case 3: // Page 3 displays the proportional gain.
      lcd.setCursor(0, 0);
      lcd.print("KP:");
      lcd.print(pGain);
      lcd.setCursor(0, 1);
      break;
    case 4: // Page 4 displays the integral gain.
      lcd.setCursor(0, 0);
      lcd.print("KI:");
      lcd.print(iGain);
      lcd.setCursor(0, 1);
      break;
  }

  // Display the error at the bottom of all pages.
  lcd.setCursor(0, 1);
  lcd.print("ERR:");
  lcd.print(error);

  // Short delay to avoid screen flicker.
  delay(100);
}