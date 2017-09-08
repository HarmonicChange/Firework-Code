#include <Servo.h>
#include "Robot.h"
int lineMidLeft, lineMidRight, lineRight, lineLeft; //Line Sensor Values Variables
int lineMidLeftPin, lineMidRightPin, lineRightPin, lineLeftPin; //Analog pins with line sensors
int toleranceForward = 100;
int blackDetect = 850; //Under this value means robot is on top of whiteish, above this value means is blackish
char turn; //0, 1, 2, 3 for left - 4, 5, 6, 7 for right
char turnFlag; //Must reach 2

void setup() {
  Serial.begin(9600);  //Debug
  turn = 0;
  turnFlag = 0;
  //Setup servos, pin 3 left wheel, pin 5 right wheel
  leftWheel.attach(3);
  rightWheel.attach(5);

  lineMidLeftPin = A1;
  lineMidRightPin = A2;
  lineRightPin = A3;
  lineLeftPin = A0;
}

void loop() {
  //Read line sensor values
  lineMidLeft = analogRead(lineMidLeftPin); lineMidRight = analogRead(lineMidRightPin);
  lineLeft = analogRead(lineLeftPin); lineRight = analogRead(lineRightPin);
  if (turn == 8) turn == 0; //Reset turn to 0 if it finishes 8 turns (4 left and 4 right)
  turnFlag = 0;

  //Detect a crossroads
  if (lineLeft > blackDetect && lineRight > blackDetect) { //left and right sensors
    if (turn < 4) { //We want to turn left for the first 4 turns
      while (turnFlag < 2) {
        turnLeft(); //Use turnLeft instead of walkLeft for fast turn
        if (lineLeft < 800) {
          turnFlag = 1;
        }
        if (turnFlag == 1 && lineLeft  > 850) {
          turnFlag = 2;
        }
        turn++;
      }
    } else { //Now we want to turn right for the next 4 turns
      while (turnFlag < 2) {
        turnRight();
        if (lineLeft < 800) {
          turnFlag = 1;
        }
        if (turnFlag == 1 && lineLeft > 850) {
          turnFlag = 2;
        }
        turn++;
      }
    }
  }

  //Now robot is on top of black line, if two sensors different less than a tolerance, walk forward
  else if ((abs(lineMidLeft - lineMidRight) < toleranceForward)) {
    walkForward();
  }
  //Otherwises, if sensor to the left has higher value than sensor to the right
  //it means robot is hitting white space on the right, so we turn left
  else if (lineMidLeft >= lineMidRight) {
    walkLeft();
  }
  else if (lineMidLeft < lineMidRight) {
    walkRight();
  }
}
