#include <AccelStepper.h> 
#include <MultiStepper.h> 

#include <AFMotor.h>

#define X_STEP  A0
#define X_DIR   A1
#define X_EN    38
#define Y_STEP  A6  //high=avance d'un pas
#define Y_DIR   A7  //high=avance low=recule
#define Y_EN    A2

// Motor shield has two motor ports, now we'll wrap them in an AccelStepper object

AccelStepper stepper1(AccelStepper::DRIVER, X_STEP, X_DIR);
AccelStepper stepper2(AccelStepper::DRIVER, Y_STEP, Y_DIR);
MultiStepper steppers;

short msg = 0;
int incomingByte = 0;

void setup()
{  
    stepper1.setMaxSpeed(1000.0);
    stepper1.setAcceleration(500.0);
    stepper1.setSpeed(1000);
    
    stepper2.setMaxSpeed(1000.0);
    stepper2.setAcceleration(500.0);
    stepper2.setSpeed(1000);
    
    stepper1.setEnablePin(X_EN);
    stepper2.setEnablePin(Y_EN);
    
    steppers.addStepper(stepper1);
    steppers.addStepper(stepper2);
    digitalWrite(X_EN, LOW);
    digitalWrite(Y_EN, LOW);
   
}
 
 
void loop()
{



  if (msg == 1) {
    stepper1.moveTo(1000);
    stepper1.runSpeedToPosition();
    Serial.print("Je tourne");
    msg = 0;
  }

  steppers.moveTo(1000);
  steppers.runSpeedToPosition(); // Blocks until all are in position
  delay(1000);
  
}

void readSerial() {
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    Serial.print("Received: ");
    Serial.println(incomingByte, DEC);
  }
}