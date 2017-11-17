#include <Servo.h>
#include "Node.h"
#include "Explorer.h"
#include "WallTurn.h"

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

int mux_S0 = 4, mux_S1 = 5;
uint8_t status = 0;

//-----DFS related control variables----Rearrange if desired//

int currPos; //The coordinate number of the current intersection the robot is on
Direction currDir; //The orientation of the robot
Node* grid[20]; //4x5 maze representation
Explorer* explorerPtr = NULL; //Initialize an Explorer object
int nextPos; //The coord of the intersection we want to travel to next. Should facilitate determining which direction to turn
Direction nextDir; // Direction robot should be facing next
Node* nextNode; //next node to travel to

// Wall sensors input values. Change name if necessary
int left, right, front;

//-----End of DFS control vars-----//

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

  //Setup pins for wall sensors.

  /*DFS: initialize all nodes, starting node, and starting direction.*/
  
  //Current position and direction facing. NOTE: needs to updated appropriately after every turn/moving to a new node.
  currPos = 19;
  currDir = north;
  
  //Initialize all nodes
  for(int i = 0; i < 20; i++) {
    grid[i] = new Node(i);
  }
}

void loop() {
  // put your main code here, to run repeatedly:


  //////------DFS-----/////// Incomplete
  
  //Checks left sensor, add neighbor if no wall and unexplored
  if (left==1){ //Wall present
    grid[currPos]->addWall(Direction((currDir-1)%4), true);
  } else{
    grid[currPos]->addWall(Direction((currDir-1)%4), false);
    grid[currPos]->addNeighbor(grid[grid[currPos]->neighborCoord(currDir, 0, currPos)]); //Calls neighborCoord first to determine coord of adjacent node,
                                                                                         //then add to neighbor array
  }

  //Checks right sensor, add neighbor if no wall and unexplored
  if (right==1){ //Wall present
    grid[currPos]->addWall(Direction((currDir+1)%4), true);
  } else{ //No wall
    grid[currPos]->addWall(Direction((currDir+1)%4), false);
    grid[currPos]->addNeighbor(grid[grid[currPos]->neighborCoord(currDir, 1, currPos)]);
  }

   //Checks front sensor, add neighbor if no wall and unexplored
  if (front==1){ //Wall present
    grid[currPos]->addWall(Direction(currDir), true);
  } else{ //No wall
    grid[currPos]->addWall(Direction(currDir), false);
    grid[currPos]->addNeighbor(grid[grid[currPos]->neighborCoord(currDir, 2, currPos)]);
  }

  nextNode = explorerPtr->nextNode();
  nextPos  = nextNode->getCoord();
  //nextDir = if currDir is north and u turn left then = west, right = east, backward = south errr we have a deadend problem...

  //IMPORTANT: make turns, etc, before updating the tree structure and current positiong and direction
  explorerPtr->travelTo(grid[nextPos]);
  currPos = nextPos;
  currDir = nextDir;
  
  //////-----End of DFS-----///////

  
  printSensors();
  if (!startFlag){
    if (digitalRead(startPin) == HIGH) startFlag = true; 
    leftWheel.write(92);  //Stop left wheel moving
    rightWheel.write(90); //Stop right wheel forward
  }
  if (startFlag){
     // Detect an intersections
      if (lineLeft > blackDetect && lineRight > blackDetect) {     
//        status = wall(); //returns an eight bit situation of the wall to status
        //now what
        //choose action
      }
      
    //Line following
      if ((abs(lineMidLeft - lineMidRight) < toleranceForward))walkForward();
      // If not on the black line, readjust
      else if (lineMidLeft >= lineMidRight) leftDrift();      
      else if (lineMidLeft < lineMidRight)  rightDrift();
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

//Return if there is a wall 
int isThereAWall (int sensor){
  if (sensor == 0){  //Y0 - left wall
      digitalWrite(mux_S0, LOW);
      digitalWrite(mux_S1, LOW);
  }
  else if (sensor == 1){ //Y1 - front wall
      digitalWrite(mux_S0, HIGH);
      digitalWrite(mux_S1, LOW);
  }
  else if (sensor == 2){ //Y2 - right wall
      digitalWrite(mux_S0, LOW);
      digitalWrite(mux_S1, HIGH);
  }
  
  ruturn (analogRead(//) < wallThreshold)
}

//0000 north south east west - front back right left
//Return the least significant 4 bits of wall status
uint8_t wall (void){
  uint8_t wallbits = 0;
  //left at bit 4, front at bit 7, right at bit 5
  wall |= isThereAwall(0) << 4; //left
  delay(10); //possible delay
  wall |= isThereAWall(1) << 7; //front
  delay(10); //possible delay
  wall |= isThereAWall(2) << 5; //right
  delay(10); //possible delay
  return wallbits;
}
