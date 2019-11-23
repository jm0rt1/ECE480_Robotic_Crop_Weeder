/*
 * Marcel Meijer
 * EC480 Team 11 Spring 2019
 * This program is used on the wireless controller kit from Adafruit in order
 * to control the Robotic Crop Weeder cart. The code processes input from the joystick/buttons 
 * and constructs a message to send to the cart.
 */

#define L_JOYSTICK A3   // Pin used for left joystick
#define R_JOYSTICK A2   // Pin used for right joystick
#define TOP_BUTTON 8
#define R_BUTTON 9 
#define BOTTOM_BUTTON 2
#define L_BUTTON 4
#define L_TRIG 6        // Pin used for left trigger
#define R_TRIG 3        // Pin used for right trigger
#define LED_PIN 13      // Pin used to display control modes
#define STICK_CLICK 5   //Click in on left analog Stick


int8_t speedLevel = 6; // used to decrease the maximum motor speed by a constant factor
int delayTrans = 25; // milliseconds of delay for transmission
int control_mode = 0; // by default control mode == 0

void setup() {

  Serial1.begin(9600); // Start serial communication with XBee at 9600 baud
  delay(10);

  Serial1.print("W7001\r\n"); // Set the bit in enable register 0x70 to 0x01

  pinMode(TOP_BUTTON,INPUT_PULLUP); // Enable pullup resistor for left trigger
  pinMode(R_BUTTON,INPUT_PULLUP);
  pinMode(BOTTOM_BUTTON,INPUT_PULLUP);
  pinMode(L_BUTTON,INPUT_PULLUP);
  pinMode(L_TRIG,INPUT_PULLUP); // Enable pullup resistor for left trigger
  pinMode(R_TRIG,INPUT_PULLUP); // Enable pullup resistor for right trigger
  pinMode(LED_PIN,OUTPUT); //Enable output pin for LED
  pinMode(STICK_CLICK, INPUT_PULLUP); //enable pullup resistor for anolog stick click
}

void loop() {
  int16_t leftStick, rightStick;    // We'll store the the analog joystick values here
  char buf0[10],buf1[10],bufTop[10]; // character buffers used to set motor speeds
  int tempVal = 1;

  // Read joysticks
  leftStick = analogRead(L_JOYSTICK);
  leftStick = map (leftStick, 0, 1023, -128, 127);
  rightStick = analogRead(R_JOYSTICK);
  leftStick = leftStick / speedLevel; // Reduce top speed

  if (digitalRead(STICK_CLICK) == 0) {
    if (control_mode == 0) {
      control_mode = 1;
      digitalWrite(LED_PIN, HIGH);
    } else {
      control_mode = 0;
      digitalWrite(LED_PIN, LOW);
    }
    delay(500); //go back to the top of the loop, dont send any messages if control mode was changed
  }

  
  // build button message buffer
  //added a switch statement that will alter the messages sent to the controller
  //based on what control mode we are in. Currently it sends the same messages either way.
  if (control_mode == 0) {

      if(digitalRead(TOP_BUTTON) == 0) {// Camera Rotation outwards (up)
        sprintf(bufTop," C1 >",tempVal);
        delay(2);
      } else if(digitalRead(R_BUTTON) == 0) { //camera rotation CW
        sprintf(bufTop," C2 >");
        delay(2);
      } else if(digitalRead(BOTTOM_BUTTON) == 0) { // camera rotation inwards (down)
        sprintf(bufTop," C3 >");
        delay(2);
      } else if(digitalRead(L_BUTTON) == 0) { //camera roation CCW
        sprintf(bufTop," C4 >");
        delay(2); //change
      } else if(digitalRead(L_TRIG) == 0) { //sends camera to a home position
        sprintf(bufTop," C5 >");
        delay(2);
      } else if(digitalRead(R_TRIG) == 0) { //used for nothing 
        sprintf(bufTop," C6 >");
        delay(2);
      } else {
        sprintf(bufTop," C0 >");
        delay(2);
      }
  } else if(control_mode == 1) {
      //X == no action
      if(digitalRead(TOP_BUTTON) == 0) {//new message for the z-rails (move up)
        sprintf(bufTop," Z1 >");
        delay(2);
      } else if(digitalRead(R_BUTTON) == 0) { //new message
        sprintf(bufTop," X >");
        delay(2);
      } else if(digitalRead(BOTTOM_BUTTON) == 0) { // new message for the z-rails (move down)
        sprintf(bufTop," Z2 >");
        delay(2);
      } else if(digitalRead(L_BUTTON) == 0) { // new message
        sprintf(bufTop," X >");
        delay(2);
      } else if(digitalRead(L_TRIG) == 0) { //new message
        sprintf(bufTop," P1 >");
        delay(2);
      } else if(digitalRead(R_TRIG) == 0) { //new message
        sprintf(bufTop," P2 >");
        delay(2);
      } else {
        sprintf(bufTop," X >");
        delay(2);
      }
  }

  // Build motor buffer
  sprintf(buf0,"<D%d ",leftStick); //start of the message

  // Build steer buffer
  if(rightStick > 0) {
    sprintf(buf1,"S%d ",rightStick);
  }
  else {
    sprintf(buf1,"S%d ",abs(rightStick));
  }

  // Send motor speeds
  delay(delayTrans);
  Serial1.print(buf0);
  delay(delayTrans);
  Serial1.print(buf1);
  delay(delayTrans);
  Serial1.print(bufTop);
}
