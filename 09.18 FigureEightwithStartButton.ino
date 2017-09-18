#include <Servo.h>
Servo leftWheel, rightWheel;
int startPin = 2;
int lineMidLeft, lineMidRight, lineRight, lineLeft; //Line Sensor Values Variables
int lineMidLeftPin, lineMidRightPin, lineRightPin, lineLeftPin; //Analog pins with line sensors
int toleranceForward = 200;
int blackDetect = 600; //Under this value means robot is on top of whiteish, above this value means is blackish
char turn; //0, 1, 2, 3 for left - 4, 5, 6, 7 for right
char turnFlag; //Must reach 2
boolean startFlag = false;

void setup() {
  Serial.begin(9600);  //Debug
  pinMode(startPin, INPUT);
  pinMode(13, OUTPUT);
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
  if (!startFlag){
    if (digitalRead(startPin)==HIGH)startFlag = true;
    //digitalWrite(13, LOW);
    leftWheel.write(92); //Stop left wheel moving
    rightWheel.write(90); //Stop right wheel forward
  }
  
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

  if (startFlag){
      //digitalWrite(13, HIGH);
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
            if (lineRight < blackDetect && turnFlag < 1) {
              turnFlag = 1;
            }
            else if (turnFlag == 1 && lineLeft  < blackDetect) {
              turnFlag = 2;
            }
            else if (turnFlag == 2 && lineMidLeft  > blackDetect) {
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
            if (lineLeft < blackDetect && turnFlag < 1) {
              turnFlag = 1;
            }
            else if (turnFlag == 1 && lineRight  < blackDetect) {
              turnFlag = 2;
            }
            else if (turnFlag == 2 && lineMidRight  > blackDetect) {
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
}

void walkForward(){ //Moving forward full speed
  leftWheel.write(137);  //137-92 = 45
  rightWheel.write(45);
}

void walkLeft(){ //Slow turn: turn left on just one wheel
  leftWheel.write(92); //Stop left wheel moving
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
