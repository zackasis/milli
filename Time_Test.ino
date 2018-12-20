//Timing Test - for Module Demo

#include <AccelStepper.h>
#include <MultiStepper.h>

// Defining each of the steppers
AccelStepper stepperX (AccelStepper::DRIVER, 3, 4); // sets to 2 pin driver on pins 3 and 4
AccelStepper stepperY (AccelStepper::DRIVER, 5, 6); // sets to 2 pin driver on pins 3 and 4
AccelStepper stepperZ (AccelStepper::DRIVER, 7, 8); // sets to 2 pin driver on pins 3 and 4

// Defining the group of steppers that are going to be managed.
MultiStepper steppers;

float stepsPerSec;
float rpm;
bool stringComplete;
long positions[3]; // Array for the go to positions
String inputString; //Input from Serial with the different positions

int xSwitch = 11;
int ySwitch = 10;
int zSwitch = 9;

float timeIn;

////////////////

void setup()
{
  Serial.begin(115200);
  // Configure each stepper
  stepperX.setMaxSpeed(1000);
  stepperY.setMaxSpeed(1000);
  stepperZ.setMaxSpeed(1000);

  // Then give them to MultiStepper to manage
  steppers.addStepper(stepperX);
  steppers.addStepper(stepperY);
  steppers.addStepper(stepperZ);

  stepperX.setAcceleration(1000);
  stepperY.setAcceleration(1000);
  stepperZ.setAcceleration(1000);

  pinMode(xSwitch, INPUT_PULLUP);
  pinMode(ySwitch, INPUT_PULLUP);
  pinMode(zSwitch, INPUT_PULLUP);

  homing();

  stepperX.setMaxSpeed(1000);
  stepperY.setMaxSpeed(1000);
  stepperZ.setMaxSpeed(500);

  positions[0] = 0;
  positions[1] = 0;
  positions[2] = 0;



}
void homing() {
  while (digitalRead(xSwitch)) {

    stepperX.move(-100);
    stepperX.runSpeed();
  }
  stepperX.setCurrentPosition(0);

  while (digitalRead(ySwitch)) {

    stepperY.move(-100);
    stepperY.runSpeed();
  }
  stepperY.setCurrentPosition(0);

  while (digitalRead(zSwitch)) {

    stepperZ.move(-100);
    stepperZ.runSpeed();
  }
  stepperZ.setCurrentPosition(0);


}

void loop()
{
  // the input from the user is done so free to move the motors
  if (stringComplete) {
    float timeStart = micros();
    steppers.moveTo(positions);
    steppers.runSpeedToPosition(); // Blocks until all are in position
    float timeEnd = micros();
    stringComplete = false;

    //print the times 
    Serial.print("Latency: ");
    Serial.print((timeStart-timeIn)/1000.0);
    Serial.println("ms");

    Serial.print("Movement: ");
    Serial.print((timeEnd-timeIn)/1000000.0);
    Serial.println("s");
  }

}

//takes in the input string and divides it the components 
void serialEvent() {

  // get the time of when it starts reading the signal
  timeIn = micros();
  
  while (Serial.available()) {

    
    
    // get the new byte:
    char inChar = (char)Serial.read();

    //the x in the string signals the end of the x position
    if (inChar == 'x') {
      float xPos = inputString.toFloat(); //sets the position for x
      if (xPos > 120){
        xPos = 120;
      }
      xPos = round(xPos/0.04);
      positions[0] = xPos;
      inputString = ""; // clears the string
    }
    
    //the y in the string signals the end of the y position
    else if (inChar == 'y') {
      float yPos = inputString.toFloat(); //sets the position for y
      if (yPos > 120){
        yPos = 120;
      }
      yPos = round(yPos/0.04);
      positions[1] = yPos;
      inputString = ""; // clears the string
    }
    
    //the z in the string signals the end of the z position
    else if (inChar == 'z') {
      float zPos = inputString.toFloat(); //sets the position for z
      if (zPos > 128){
        zPos = 128;
      }
      zPos = round(zPos/0.04);
      positions[2] = zPos;
      inputString = ""; // clears the string
    }

    // if not the end it adds the character to the string:
    else {
      inputString += inChar;
    }

    // if the incoming character is a newline, sets flag to show end of input -> main loop will use it.

    if (inChar == '\n') {
      stringComplete = true;
    }

  }
}
