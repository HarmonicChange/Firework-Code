#include <Servo.h>
Servo leftWheel, rightWheel;
int startPin = 2;
int lineMidLeft, lineMidRight, lineRight, lineLeft;             //Line Sensor Values Variables
int lineMidLeftPin, lineMidRightPin, lineRightPin, lineLeftPin; // Analog pins with line sensors
int toleranceForward = 200; //
int blackDetect = 600;      // Threshold above which sensors are reading a black line
int turn = 0;               // Determines stage of 8-figure turn, values 0-7
boolean startFlag = false;  // Set by pressing button, starts operation of robot.
int frontDistance;
int frontIR;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);  //Debug
  pinMode(startPin, INPUT);
  // Initialize system variables
  //Setup servos, pin 5 left wheel, pin 3 right wheel
  leftWheel.attach(5);
  rightWheel.attach(3);
  lineMidLeftPin = A1;
  lineMidRightPin = A2;
  lineRightPin = A3;
  lineLeftPin = A0;
  frontIR = A4;
}

void loop() {
  // put your main code here, to run repeatedly:
  frontDistance = analogRead(frontIR);
  Serial.println(frontDistance);
  printSensors();
  if (!startFlag){
    if (digitalRead(startPin) == HIGH) startFlag = true; 
    leftWheel.write(92);  //Stop left wheel moving
    rightWheel.write(90); //Stop right wheel forward
  }
  if (startFlag){
    if (frontDistance < 300){ //Keep walking when distance is greater than 8cm
      if ((abs(lineMidLeft - lineMidRight) < toleranceForward))walkForward();
      // If not on the black line, readjust
      else if (lineMidLeft >= lineMidRight) leftDrift();      
      else if (lineMidLeft < lineMidRight)  rightDrift();
    }
    else{               //Turn right when it is 300 cm away from the wall
      rightTurn();
    }
  }
}


//5cm         419
//10cm        230
//15cm        160
//20cm        120
//25cm        95
//30cm        70
//35cm        60
//40cm        48
//45cm        45
//50cm        40
//60cm        30
//70cm        16
 
//For the wood, up to 3cm accuracy
void printSensors(){
    // Used for debugging
    // Prints line sensor values left to right
    lineLeft = analogRead(lineLeftPin);
    lineMidLeft = analogRead(lineMidLeftPin);
    lineMidRight = analogRead(lineMidRightPin);
    lineRight = analogRead(lineRightPin);
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

