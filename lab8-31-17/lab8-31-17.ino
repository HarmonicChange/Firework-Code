#include <Servo.h>

int ledPin;
int potInput;
Servo myservo;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  //pinMode(LED_BUILTIN, OUTPUT);
  ledPin = 3;
  potInput = A0;
  myservo.attach(3);
  
  pinMode(ledPin, OUTPUT);
  pinMode(potInput, INPUT);
  
  //Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {
  //  digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  //  delay(1000);                       // wait for a second
  //  digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
  //  delay(1000);                       // wait for a second

  
//  Serial.println(analogRead(potInput));
//  delay(500);

//  int val = analogRead(potInput);
//  analogWrite(ledPin, val/4);
//  delay(100);

// Test servo
//  myservo.write(180); //CCW
//  delay (1000);
//  myservo.write(90);
//  delay(1000);
//  myservo.write(0); //CW
//  delay (1000);

//Servo with pot
  int val = analogRead(potInput);
  myservo.write(val/5.7);
  delay(100);
}
