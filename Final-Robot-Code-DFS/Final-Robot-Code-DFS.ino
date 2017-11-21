#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>

#include <Servo.h>
#include "Node.h"
#include "Explorer.h"
#include <SPI.h>
#include "printf.h"

Servo leftWheel, rightWheel;
int startPin = 4;
int lineMidLeft, lineMidRight, lineRight, lineLeft;             //Line Sensor Values Variables
int lineMidLeftPin, lineMidRightPin, lineRightPin, lineLeftPin; // Analog pins with line sensors
int toleranceForward = 200; //
int blackDetect = 600;      // Threshold above which sensors are reading a black line
int distanceInput; //wall sensor input pin, controlled by mux
int IRInput; //treasure detector input pin, controlled by mux 
int mux_S0 = 7, mux_S1 = 8; //00 = left, 10 = front, 01 = right
int intRdy = 1;

// RF Transmitter variables
RF24 radio(9,10);  // Set up nRF24L01 radio on SPI bus plus pins 9 & 10
const uint64_t pipes[2] = { 0x000000002ALL, 0x000000002BLL };  // Radio pipe addresses for the 2 nodes to communicate.
int maze[] = 
  {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,  
  };

// DFS related control variables
int currPos; //The coordinate number of the current intersection the robot is on
Direction currDir; //The orientation of the robot
Direction nextDir; //The orientation of the robot should assume next
Node* grid[20]; //4x5 maze representation
Explorer* explorerPtr = NULL; //Initialize an Explorer object
int nextPos; //The coord of the intersection we want to travel to next. Should facilitate determining which direction to turn
Node* nextNode; //next node to travel to

void setup() {
  initializeStuff();
  initializeRFStuff();

  //Starting position and direction
  currPos = 19;
  currDir = north;

  waitForStart();
}

void loop() {
  updateLineSensors(); 
  printSensors(); //used for debugging
  

  // Line following (not at an intersection)
  if ((abs(lineMidLeft - lineMidRight) < toleranceForward)){
    //Serial.println("Walking forward");
    walkForward();
  }
  // If not on the black line, readjust
  else if (lineMidLeft >= lineMidRight) {
    //Serial.println("Left drifting");
    leftDrift();      
  }
  else if (lineMidLeft < lineMidRight) { 
    //Serial.println("Right drifting");
    rightDrift();}

  //Not at intersection
  if (lineLeft < blackDetect && lineRight < blackDetect) intRdy = 1;
  
  // At an intersection
  if (lineLeft > blackDetect && lineRight > blackDetect && intRdy) {
    intRdy = 0;
    Serial.print("Intersection detected:  ");
    Serial.println(int(currPos));
    Serial.print("Direction:  ");
    Serial.println(int(currDir));
    intersect(); //Stops the robot to take wall reading samples
    
    nextNode = explorerPtr->nextNode();
    nextPos  = nextNode->getCoord();
    Serial.print("currPos:");
    Serial.println(int(currPos));
    Serial.print("nextPos:");
    Serial.println(int(nextPos));

    keepStraight(); //Restart the walking forward, since intersect disabled it
    turn(getTurn());
    
    updateRobotLocation(); 
    //nextDir = if currDir is north and u turn left then = west, right = east, backward = south
  
    explorerPtr->travelTo(grid[nextPos]);
    currPos = nextPos;
    //currDir = nextDir;
    //Robot should be following the line toward this next node now
  }    

  if(explorerPtr->isDone()) {
    leftWheel.write(92);
    rightWheel.write(90);
    Serial.println("We're done!");
    while(1);
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
