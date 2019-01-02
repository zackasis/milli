#include <AccelStepper.h>
#include<MultiStepper.h>

// Define each stepper
AccelStepper pitch(AccelStepper::DRIVER,3,4); // Pin order is step, dir
AccelStepper yaw(AccelStepper::DRIVER,5,6); 
AccelStepper stepperX (AccelStepper::DRIVER, 7, 8); 
AccelStepper stepperY (AccelStepper::DRIVER, 9, 10); 
AccelStepper stepperZ (AccelStepper::DRIVER, 11, 12); 

// Defining the group of steppers 
MultiStepper steppers;

// Array for the target positions
long positions[5]; 

//variables
String posString;
bool posReceived;

//step angle and microstep resolution for pitch and yaw
float stepAngle = 1.8;
int stepRes = 16; 

//set pins for limit switches
int xSwitch = 13;
int ySwitch = 14;
int zSwitch = 15;
int pitchSwitch = 16;
int yawSwitch = 17; 

/////////////////////////////


void setup()
{
  Serial.begin(115200);

  //Position update interrupt pin
  attachInterrupt(digitalPinToInterrupt(2), PosUpdate, CHANGE);
  pinMode(2, INPUT_PULLUP);

  //limit switches
  pinMode(xSwitch, INPUT_PULLUP);
  pinMode(ySwitch, INPUT_PULLUP);
  pinMode(zSwitch, INPUT_PULLUP);
  pinMode(pitchSwitch, INPUT_PULLUP);
  pinMode(yawSwitch, INPUT_PULLUP);

  //set max speeds
  stepperX.setMaxSpeed(1000);
  stepperY.setMaxSpeed(1000);
  stepperZ.setMaxSpeed(1000);
  pitch.setMaxSpeed(200*stepRes);
  yaw.setMaxSpeed(200*stepRes);

  //set accelerations
  stepperX.setAcceleration(1000);
  stepperY.setAcceleration(1000);
  stepperZ.setAcceleration(1000);
  pitch.setAcceleration(200*stepRes); 
  yaw.setAcceleration(200*stepRes); 
  
  //add steppers to the group
  steppers.addStepper(stepperX);
  steppers.addStepper(stepperY);
  steppers.addStepper(stepperZ);
  steppers.addStepper(pitch);
  steppers.addStepper(yaw);

  //Homing sequence function
  Home();

  //set zero positions for all motors
  positions[0] = 0; //x
  positions[1] = 0; //y
  positions[2] = 0; //z
  positions[3] = 0; //pitch
  positions[4] = 0; //yaw
 
}

void loop()
{
   steppers.moveTo(positions);    //set target positions
   steppers.run();                //does not 
}


void Home()
{
  while (digitalRead(xSwitch)) 
  {
    stepperX.move(-100);
    stepperX.runSpeed();
  }
  stepperX.stop();
  stepperX.setCurrentPosition(0);

  while (digitalRead(ySwitch)) 
  {
    stepperY.move(-100);
    stepperY.runSpeed();
  }
  stepperY.stop();
  stepperY.setCurrentPosition(0);

  while (digitalRead(zSwitch)) 
  {
    stepperZ.move(-100);
    stepperZ.runSpeed();
  }
  stepperZ.stop();
  stepperZ.setCurrentPosition(0);

  while (digitalRead(pitchSwitch))
  {
   pitch.setSpeed(178);
   pitch.move(-200); 
   pitch.runSpeedToPosition();
  }
  pitch.stop();
  pitch.setCurrentPosition(0);
  
}


void PosUpdate()
{ 
   while(Serial.available()) 
  {
    char inChar = (char)Serial.read();
    
    if (inChar == 'x') 
    {
      float xPos = posString.toFloat(); 
      if (xPos > 120)
      {
        xPos = 120;
      }
      xPos = round(xPos/0.04);
      positions[0] = xPos;      //set x position
      posString = "";           //clear string
    }
    else if (inChar == 'y') 
    {
      float yPos = posString.toFloat(); 
      if (yPos > 120)
      {
        yPos = 120;
      }
      yPos = round(yPos/0.04);
      positions[1] = yPos;      //set y position
      posString = "";           //clear string
    }
    else if (inChar == 'z') 
    {
      float zPos = posString.toFloat(); //
      if (zPos > 128)
      {
        zPos = 128;
      }
      zPos = round(zPos/0.04);
      positions[2] = zPos;      //set z position
      posString = "";           //clear string
    }
    else if (inChar == 'p') 
    {
     if (posString.toInt() < 110)
      {
       positions[3] = (int) round(posString.toInt()*stepRes/stepAngle); //set pitch angle
      }
     posString = "";            //clear string
    }
    else if (inChar == 'y')
    {
     if (posString.toInt() < 130)
     {
      positions[4] = (int) round(posString.toInt()*stepRes/stepAngle);  //set yaw angle
     }
     posString = "";            //clear string
    }
    else
    {
      posString += inChar;
    }
  }
}



