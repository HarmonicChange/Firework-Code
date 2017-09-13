#include <Servo.h>
Servo leftWheel, rightWheel;
int lineMidLeft, lineMidRight, lineRight, lineLeft; //Line Sensor Values Variables
int lineMidLeftPin, lineMidRightPin, lineRightPin, lineLeftPin; //Analog pins with line sensors
int toleranceForward = 200;
int blackDetect = 800; //Under this value means robot is on top of whiteish, above this value means is blackish
char turn; //0, 1, 2, 3 for left - 4, 5, 6, 7 for right
char turnFlag; //Must reach 2

void setup() {
  Serial.begin(9600);  //Debug
  turn = 0;
  turnFlag = 0;
  //Setup servos, pin 5 left wheel, pin 3 right wheel
  leftWheel.attach(5);
  rightWheel.attach(3);

  lineMidLeftPin = A1;
  lineMidRightPin = A2;
  lineRightPin = A3;
  lineLeftPin = A0;
}

void loop() {
  //Read line sensor values
  lineMidLeft = analogRead(lineMidLeftPin); lineMidRight = analogRead(lineMidRightPin);
  lineLeft = analogRead(lineLeftPin); lineRight = analogRead(lineRightPin);

  //Debug
  Serial.print(lineLeft);
  Serial.print("  ");
  Serial.print(lineMidLeft);
  Serial.print("  ");
  Serial.print(lineMidRight);
  Serial.print("  ");
  Serial.print(lineRight);
  Serial.print("  ");
  Serial.println();
  
  if (turn == 8) turn = 0; //Reset turn to 0 if it finishes 8 turns (4 left and 4 right)
  turnFlag = 0;

  //Detect a crossroads
  if (lineLeft > blackDetect && lineRight > blackDetect) { //left and right sensors
    if (turn < 4) { //We want to turn left for the first 4 turns
      while (turnFlag < 3) {
        walkLeft(); //Begin turning left
        
        //Update needed sensor values
        lineRight = analogRead(lineRightPin);
        lineLeft = analogRead(lineLeftPin);
        lineMidLeft = analogRead(lineMidLeftPin);

        //Test necessary conditions for completion of left turn
        if (lineRight < 800 && turnFlag < 1) {
          turnFlag = 1;
        }
        else if (turnFlag == 1 && lineLeft  < 800) {
          turnFlag = 2;
        }
        else if (turnFlag == 2 && lineMidLeft  > 800) {
          turnFlag = 3;
          turn++;
        }
      }
    } else { //Now we want to turn right for the next 4 turns
      while (turnFlag < 3) {
        walkRight(); ///Begin turning right

        //Update needed sensor values
        lineLeft = analogRead(lineLeftPin);
        lineRight = analogRead(lineRightPin);
        lineMidRight = analogRead(lineMidRightPin);

        //Test necessary conditions for completion of right turn
        if (lineLeft < 800 && turnFlag < 1) {
          turnFlag = 1;
        }
        else if (turnFlag == 1 && lineRight  < 800) {
          turnFlag = 2;
        }
        else if (turnFlag == 2 && lineMidRight  > 800) {
          turnFlag = 3;
          turn++;
        }
      }
    }
  }
  
  //Now robot is on top of black line, if two sensors different less than a tolerance, walk forward
  else if ((abs(lineMidLeft - lineMidRight) < toleranceForward)) {
    walkForward();
    Serial.println("straight");
  }
  //Otherwises, if sensor to the left has higher value than sensor to the right
  //it means robot is hitting white space on the right, so we turn left
  else if (lineMidLeft >= lineMidRight) {
    walkLeft();
    Serial.println("left");
  }
  else if (lineMidLeft < lineMidRight) {
    walkRight();
    Serial.println("right");
  }
}

void walkForward(){ //Moving forward full speed
  leftWheel.write(180);
  rightWheel.write(0);
}

void walkLeft(){ //Slow turn: turn left on just one wheel
  leftWheel.write(90); //Stop left wheel moving
  rightWheel.write(0); //Move right wheel forward
}

void walkRight(){ //Slow turn: turn right on one wheel
  leftWheel.write(180); //Move left wheel forward
  rightWheel.write(90); //Stop right wheel moving
}

void turnLeft(){ //Fast turn: turn left on both wheels
  leftWheel.write(0);   //Move left wheel backward
  rightWheel.write(0);  //Move right wheel forward
}

void turnRight(){ //Fast turn: turn right on both wheels
  leftWheel.write(180); //Move left wheel forward
  rightWheel.write(180);//Move right wheel backward
}
