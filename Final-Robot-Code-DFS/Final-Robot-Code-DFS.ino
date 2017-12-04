#include <nRF24L01.h>
#include <RF24.h>
//#include <RF24_config.h>

#define LOG_OUT 1 // use the log output function
#define FFT_N 128 // set to 256 point fft

#include <Servo.h>
#include "Node.h"
#include "Explorer.h"
#include <SPI.h>
#include "printf.h"
#include <FFT.h>

Servo leftWheel, rightWheel;
int startPin = 4;
int mic;
int lineMidLeft, lineMidRight, lineRight, lineLeft;             //Line Sensor Values Variables
int lineMidLeftPin, lineMidRightPin, lineRightPin, lineLeftPin; // Analog pins with line sensors
int toleranceForward = 200; //
int blackDetect = 700;      // Threshold above which sensors are reading a black line
int blackDetectOuter = 700;      
int distanceInput; //wall sensor input pin, controlled by mux
int IRInput; //treasure detector input pin, controlled by mux 
int mux_S0 = 7, mux_S1 = 8; //00 = left, 10 = front, 01 = right
int intRdy = 1;
bool backTrack = false;

unsigned long lasttime = 0;
unsigned long period = 0;
uint8_t heard = 0;

int endOn = 10;

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

  //TODO Check interrupt pins
  attachInterrupt(digitalPinToInterrupt(2), treasure_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), treasure_ISR, FALLING);

  waitForStart();
}

void treasure_ISR() {
  period = micros() - lasttime  ;
  lasttime = micros();
  if(period > 130 && period < 150) {
    Serial.println("7kHz Treasure");
    maze[currPos] |= (1<<4); 
    period = 0;
  }
  else if(period > 75 && period < 85) {
    Serial.println("12kHz Treasure");
    maze[currPos] |= (1<<5);
    period = 0;
  }
  else if(period > 50 && period < 60) {
    Serial.println("17kHz Treasure");
    maze[currPos] |= (1<<4); 
    maze[currPos] |= (1<<5);
    period = 0;
  }
}

void loop() {
  updateLineSensors(); 
  //printSensors();

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
    rightDrift();
  }

  //Not at intersection
  if (lineLeft < blackDetectOuter && lineRight < blackDetectOuter) intRdy = 1;  
  
  // At an intersection
  if (lineLeft > blackDetectOuter && lineMidLeft > blackDetect && lineMidRight > blackDetect && lineRight > blackDetectOuter && intRdy) {
    leftWheel.write(91); 
    rightWheel.write(90);
    intRdy = 0;
    Serial.println("------------------");
    printSensors();
    Serial.print("Intersection detected:  ");
    Serial.println(int(currPos));
    Serial.print("Direction:  ");
    Serial.println(int(currDir));
    //if (not (grid[nextPos]->isExplored()))
    //if (intersect()){ //Stops the robot to take wall samples   
    intersect();
    
    Serial.print("currPos:");
    Serial.println(int(currPos));
    Serial.print("nextPos:");
    Serial.println(int(nextPos));

    if (nextPos==currPos) { //isDone
      leftWheel.write(91);
      rightWheel.write(90);
      
      Serial.println("We're done!");
      while(1){}
    }
    
    keepStraight(); //Restart the walking forward, since intersect disabled it
    turn(getTurn());

    
    
    //updateRobotLocation(); 
  
    explorerPtr->travelTo(grid[nextPos]);
    currPos = nextPos;
      //Robot should be following the line toward this next node now
    //}
    //else {keepStraight();}
    
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
