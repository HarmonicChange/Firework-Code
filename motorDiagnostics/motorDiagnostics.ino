#include <Servo.h>

Servo leftWheel, rightWheel;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  leftWheel.attach(5);
  rightWheel.attach(6);

  leftWheel.write(91);
  rightWheel.write(90);
}

void loop() {
  // put your main code here, to run repeatedly:

}
