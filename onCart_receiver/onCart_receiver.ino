/*   Marcel Meijer / Shulin Xiang
     ECE480 Team 11 Spring 2019
     This code is used on the Arduino to receive information
     from the wireless controller and update the cart's systems
     as needed  */
#include <SabertoothSimplified.h>
#include <Servo.h>

#define SOLENOID 13
#define IGNITER 12

Servo servo_top; // top servo
Servo servo_bottom; // bottom servo

SabertoothSimplified ST;

const int stepPin = 5;
const int dirPin = 2;
const int enPin = 8;

bool started= false;//True: Message is strated
bool ended  = false;//True: Message is finished 
char incomingByte ; //Variable to store the incoming byte
char msg[40];    //Message
char drive_[10]; // store drive value
char steer[10]; // store steer value
char buttonTop[10]; // store button value
char propaneBuffer[10];
byte topIndex; // index of button array
byte propaneIndex; //index of propane array
byte index;     //Index of array
byte driveIndex; // index of drive array
byte steerIndex; // index of steer array
char tempChar;

int prevDriveVal = 0;
int prevButton;
// set initial values of servo
int servoTopAngle = 90;
int servoBottomAngle = 74;
bool servoUp = false;

void setup() {
  //Start the serial communication
  SabertoothTXPinSerial.begin(9600);
  Serial.begin(9600); //Baud rate must be the same as is on xBee module
  // attach top servo to pin 9 and initialize its angle
  servo_top.attach(9);
  servo_top.write(servoTopAngle);
  // attach bottom servo to pin 10 and initialize its angle
  servo_bottom.attach(10);
  servo_bottom.write(servoBottomAngle);
  
  // set output modes for stepper motor controller
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  pinMode(enPin,OUTPUT);
  pinMode(SOLENOID, OUTPUT);
  pinMode(IGNITER, OUTPUT);
  digitalWrite(enPin,LOW);
}

void loop() {
  
  while (Serial.available()>0){
    //Read the incoming byte
    incomingByte = Serial.read();
    //Start the message when the '<' symbol is received
    if(incomingByte == '<') {
      started = true;
      ended = false;
      index = 0;
      driveIndex = 0;
      propaneIndex = 0;
      drive_[driveIndex] = '\0'; // throw away any incomplete packet
      steerIndex = 0;
      steer[steerIndex] = '\0';
      propaneBuffer[propaneIndex] = '\0';
      msg[index] = '\0'; // Throw away any incomplete packet
    }
   //End the message when the '>' symbol is received
    else if(incomingByte == '>') {
      ended = true;
      break; // Done reading - exit from while loop!
    }
   
    //Read the message!
    else {
      if(index < 40) { // Make sure there is room
        msg[index] = incomingByte; // Add char to array
        index++;
        msg[index] = '\0'; // Add NULL to end //gets overwritten in each each iteration.
      }
    }
  }

 if(started && ended) {
   //int value = atoi(msg);
   //analogWrite(ledPin, value);
   char current;
   int actualDriveVal = 0;
   for (byte i = 0; i < index; i++) {
      // get current character
      current = msg[i];
      // check if it is for drive
      if (current == 'D') {
        i++;
        current = msg[i]; // get next character
        while(current != ' ') {
          // fill drive buffer
          drive_[driveIndex] = current; //read in the number
          driveIndex++; //next index
          drive_[driveIndex] = '\0'; //why end?
          i++; //next character of message.
          current = msg[i]; // "D#### "
        }
      
      // check if it is for steering
      } else if (current == 'S') {
        i++;
        current = msg[i]; // get next character
        while (current != ' ')
        {
          // fill steer buffer
          steer[steerIndex] = current;
          steerIndex++;
          steer[steerIndex] = '\0';
          i++;
          current = msg[i];
        }
      // check if it is for camera
      } else if (current == 'C') {
        i++;
        current = msg[i]; // get next character
        while (current != ' ')
        {
          // fill button character
          buttonTop[topIndex] = current;
          topIndex++;
          buttonTop[topIndex] = '\0';
          i++;
          current = msg[i];
        }
      } else if (current == 'P') { //decode propane message
        i++;
        current = msg[i]; // get next character;
        while(current != ' ') {
          // fill button character
          propaneBuffer[propaneIndex] = current;
          propaneIndex++;
          propaneBuffer[propaneIndex] = '\0';
          i++;
          current = msg[i];
        }
      }
    }
   //Serial.println(); //Only for debugging
   int driveValue = atoi(drive_); // convert drive buffer to usable value

   if (abs(driveValue) <= 10)
   {
    // this is used to give joystick a pocket so it is not so touchy when the joystick is at the center position
      driveValue = 0;
   }

   // update motor values if the value has changed
   if (driveValue != prevDriveVal)
   {
    ST.motor(1,driveValue);
    ST.motor(2,driveValue);
   }
   // update previous drive value
   prevDriveVal = driveValue;
   
   int steerValue = atoi(steer); // convert steer buffer to usable value

   if (steerValue > 750 && steerValue <= 1023)
   {
      digitalWrite(dirPin,LOW); // turn steering to the right
      for(int x = 0; x < 40; x++) {
        digitalWrite(stepPin,HIGH);
        delayMicroseconds(500);
        digitalWrite(stepPin,LOW);
        delayMicroseconds(500);
      }
   }
   else if (steerValue >= 0 && steerValue < 274)
   {
      digitalWrite(dirPin,HIGH); // turn steering to the left
      for(int x = 0; x < 40; x++) {
        digitalWrite(stepPin,HIGH);
        delayMicroseconds(500);
        digitalWrite(stepPin,LOW);
        delayMicroseconds(500);
      }
   }
   
   int topValue = atoi(buttonTop); // convert button buffer to usable value

   
   if (topValue == 1)
   {
     if (servoTopAngle < 120)
     {
       servoTopAngle += 2; // increase angle
       // set cap for angle value
       if (servoTopAngle > 120) servoTopAngle = 120;
       servo_top.write(servoTopAngle);
     }
   }
   else if (topValue == 3)
   {
     if (servoTopAngle > 9)
     {
       servoTopAngle -= 2; // decrease angle
       // set lower bound for angle
       if (servoTopAngle < 9) servoTopAngle = 9;
       servo_top.write(servoTopAngle);
     }
   }
   else if (topValue == 2)
   {
     if (servoBottomAngle < 180)
     {
       servoBottomAngle += 2; // increase angle
       // set upper bound for angle
       if (servoBottomAngle < 180) servoBottomAngle = 180;
       servo_bottom.write(servoBottomAngle);
     }
   }
   else if (topValue == 4)
   {
     if (servoBottomAngle > 9)
     {
       servoBottomAngle -= 2; // decrease angle
       // set lower bound for angle
       if (servoBottomAngle < 9) servoBottomAngle = 9;
       servo_bottom.write(servoBottomAngle);
     }
     
   }
   else if (topValue == 5){
      // switch between two actions for button
      if (prevButton != 5)
      {
        if(servoUp){
          servoTopAngle = 90;
          servo_top.write(servoTopAngle);
          servoUp = false;
        }
        else{
          servoTopAngle = 17;
          servo_top.write(servoTopAngle);
          servoUp = true;
        }
      }
      servoBottomAngle = 74;
      servo_bottom.write(servoBottomAngle);
   }
   else if (topValue == 6){

      servoBottomAngle = 74;
      servo_bottom.write(servoBottomAngle);
   }

  int propaneValue = atoi(propaneBuffer);
  if(propaneValue == 1) {
    digitalWrite(SOLENOID, HIGH);
    delay(50);
    digitalWrite(SOLENOID, LOW);
  } else if(propaneValue == 2) {
    digitalWrite(IGNITER, HIGH);
    delay(50);
    digitalWrite(IGNITER, LOW);
  }



  prevButton = topValue;
  
  topIndex = 0;
  driveIndex = 0;
  steerIndex = 0;
  propaneIndex = 0;
  buttonTop[topIndex] = '\0';
  drive_[driveIndex] = '\0';
  steer[steerIndex] = '\0';
  index = 0;
  msg[index] = '\0';
  started = false;
  ended = false;
  delay(50);
 }
}
