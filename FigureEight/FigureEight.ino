#include <Servo.h>
Servo leftWheel, rightWheel;
int startPin = 2;
int lineMidLeft, lineMidRight, lineRight, lineLeft;             //Line Sensor Values Variables
int lineMidLeftPin, lineMidRightPin, lineRightPin, lineLeftPin; // Analog pins with line sensors
int toleranceForward = 200; //
int blackDetect = 600;      // Threshold above which sensors are reading a black line
int turn = 0;               // Determines stage of 8-figure turn, values 0-7

void setup() {
  initializeStuff();
  waitForStart();
}

void loop() {
  // Update line sensor values
  updateLineSensors();
  printSensors();

  //Reset turn to 0 if it finishes 8 turns of figure eight
  if (turn == 8) turn = 0; 
  
  // Detect a crossroads
  if (lineLeft > blackDetect && lineRight > blackDetect) { 
    if (turn == 0 || turn == 4) keepStraight();
    else if (turn > 0 && turn < 4) rightTurn();
    else leftTurn();
    turn += 1;   
  }
  
  // Detects that it's on the black line
  else if ((abs(lineMidLeft - lineMidRight) < toleranceForward)) walkForward();
  
  // If not on the black line, readjust
  else if (lineMidLeft >= lineMidRight) leftDrift();      
  else if (lineMidLeft < lineMidRight)  rightDrift();

}


