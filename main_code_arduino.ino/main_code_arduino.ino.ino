#include <AccelStepper.h> 
#include <MultiStepper.h> 

#include "parser.hpp"

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
    init_msg_handler();
   
}

char output_buffer[128];

void confirm(msg_buffer* msg)
{
  if((msg->val[l_to_id('N')]).has_value)
  {
    sprintf(output_buffer, "G100 N%d\r\n", msg->val[l_to_id('N')].i_val);
    Serial.print(output_buffer);
  }
}


int handle_message(msg_buffer* msg)
{

  if(msg == NULL || (msg->val[l_to_id('G')]).present == false){msg->state=BUFF_ERROR; return 2;}
  int command = msg->val[l_to_id('G')].i_val;

  switch(command)
  {
    case 0:
    {
      if((msg->val[l_to_id('X')]).has_value)
      {
        stepper1.setSpeed((msg->val[l_to_id('X')]).i_val);  
      }
      if((msg->val[l_to_id('Y')]).has_value)
      {
        stepper2.setSpeed((msg->val[l_to_id('Y')]).i_val);  
      }
      confirm(msg);
      break;
    }
    default:
    {
      sprintf(output_buffer, "Unknown command G%d\r\n", command);
      Serial.write(output_buffer, strlen(output_buffer)+1);
      msg->state = BUFF_ERROR;
      return 1;
      break;
    }
  }
  msg->state = EMPTY;
  
  return 0;
}
 
void loop()
{
  steppers.run();
  parse_all();
  
  int pos;
  msg_buffer * message = get_message(&pos);
  while(message != NULL)
  {
    int ret_val = handle_message(message);
    message = get_message(&pos);
  }
  
}

void readSerial() {
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    Serial.print("Received: ");
    Serial.println(incomingByte, DEC);
  }
}
