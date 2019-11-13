#define SOLENOID 13
#define SOLENOID_CONTROL 2

void setup() {
    pinMode(SOLENOID, OUTPUT);
    pinMode(SOLENOID_CONTROL, INPUT);
}

void loop() {
    int val = digitalRead(SOLENOID_CONTROL);
    if(val == HIGH) {
      digitalWrite(SOLENOID, HIGH);
      delay(1000);
    }
    else {
      digitalWrite(SOLENOID, LOW);
      delay(1000); //hello dkdkdkdkdkdkd
    }
}

//comment