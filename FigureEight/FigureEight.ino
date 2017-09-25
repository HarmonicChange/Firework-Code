#include <Servo.h>
Servo leftWheel, rightWheel;
int startPin = 2;
int lineMidLeft, lineMidRight, lineRight, lineLeft;             //Line Sensor Values Variables
int lineMidLeftPin, lineMidRightPin, lineRightPin, lineLeftPin; // Analog pins with line sensors
int toleranceForward = 200; //
int blackDetect = 600;      // Threshold above which sensors are reading a black line
int turn = 0;               // Determines stage of 8-figure turn, values 0-7
boolean startFlag = false;  // Set by pressing button, starts operation of robot.

void setup() {
  // Setup Serial and pin directions
  Serial.begin(9600);
  pinMode(startPin, INPUT);
  // Initialize system variables
  //Setup servos, pin 5 left wheel, pin 3 right wheel
  leftWheel.attach(5);
  rightWheel.attach(3);
  lineMidLeftPin = A1;
  lineMidRightPin = A2;
  lineRightPin = A3;
  lineLeftPin = A0;
}

void loop() {
  if (!startFlag){
    if (digitalRead(startPin) == HIGH) startFlag = true; 
    leftWheel.write(92);  //Stop left wheel moving
    rightWheel.write(90); //Stop right wheel forward
  }
  
  //Update line sensor values
  lineMidLeft = analogRead(lineMidLeftPin); lineMidRight = analogRead(lineMidRightPin);
  lineLeft = analogRead(lineLeftPin); lineRight = analogRead(lineRightPin);
  printSensors();

  // Robot has begun
  if (startFlag){
      if (turn == 8) turn = 0; //Reset turn to 0 if it finishes 8 turns of figure eight

      // Detect a crossroads
      if (lineLeft > blackDetect && lineRight > blackDetect) { //left and right sensors
        Serial.println("TURN!");
        if (turn == 0 || turn == 4) keepStraight();
        else if (turn > 0 && turn < 4) rightTurn();
        else leftTurn();
        turn += 1;   
        Serial.println("TurnEnd");
      }
      
      // Detects that it's on the black line
      else if ((abs(lineMidLeft - lineMidRight) < toleranceForward))walkForward();

      // If not on the black line, readjust
      else if (lineMidLeft >= lineMidRight) leftDrift();      
      else if (lineMidLeft < lineMidRight)  rightDrift();
  }
}

void printSensors(){
    // Used for debugging
    // Prints line sensor values left to right
  Serial.print(lineLeft);
  Serial.print("  ");
  Serial.print(lineMidLeft);
  Serial.print("  ");
  Serial.print(lineMidRight);
  Serial.print("  ");
  Serial.print(lineRight);
  Serial.print("  ");
  Serial.print(turn);
  Serial.print("  ");
  Serial.println();
}
