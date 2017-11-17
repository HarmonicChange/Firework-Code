#include <Servo.h>
#include "Node.h"
#include "Explorer.h"

Servo leftWheel, rightWheel;
int startPin = 2;
int lineMidLeft, lineMidRight, lineRight, lineLeft;             //Line Sensor Values Variables
int lineMidLeftPin, lineMidRightPin, lineRightPin, lineLeftPin; // Analog pins with line sensors
int toleranceForward = 200; //
int blackDetect = 600;      // Threshold above which sensors are reading a black line
int distanceInput; //wall sensor input pin, controlled by mux
int IRInput; //treasure detector input pin, controlled by mux 
int mux_S0 = 7, mux_S1 = 8; //00 = left, 10 = front, 01 = right
int intRdy = 1;
//-----DFS related control variables----Rearrange if desired//

int currPos; //The coordinate number of the current intersection the robot is on
Direction currDir; //The orientation of the robot
Direction nextDir; //The orientation of the robot should assume next
Node* grid[20]; //4x5 maze representation
Explorer* explorerPtr = NULL; //Initialize an Explorer object
int nextPos; //The coord of the intersection we want to travel to next. Should facilitate determining which direction to turn
Node* nextNode; //next node to travel to

//-----End of DFS control vars-----//

//Forward declaration of all methods
void printSensors();
int isThereAWall(int);
uint8_t wall();
void leftTurn();
void rightTurn();
void keepStraight();
void walkForward();
void leftTurnSlow();
void rightTurnSlow();
void leftTurnFast();
void rightTurnFast();
void leftDrift();
void rightDrift();
int getTurn();

void setup() {
  Serial.begin(9600);  //Debug
  pinMode(startPin, INPUT);
  pinMode(mux_S0, OUTPUT);
  pinMode(mux_S1, OUTPUT);
  // Initialize system variables
  //Setup servos, pin 5 left wheel, pin 3 right wheel
  leftWheel.attach(5);
  rightWheel.attach(6);
  lineMidLeftPin = A1;
  lineMidRightPin = A2;
  lineRightPin = A3;
  lineLeftPin = A0;

  //Setup pins for wall/distance sensors.
  distanceInput = A4;
  //IRInput = ?;
  
  //Starting position and direction
  currPos = 19;
  currDir = north;


  
  //Initialize all nodes
  for(int i = 0; i < 20; i++) {
    grid[i] = new Node(i);
  }

    //Initialize explorer
  explorerPtr = new Explorer(grid[19]);

  /*Wait until starting signal - tone or button*/
  while(digitalRead(startPin) == LOW){
    leftWheel.write(92);
    rightWheel.write(90);
    //Serial.println("Waiting");
    } //Wait while startPin is still low
}

void loop() {
  printSensors(); //used for debugging

  //Line following (not at an intersection)
  if ((abs(lineMidLeft - lineMidRight) < toleranceForward)){
    //Serial.println("Wlking forward");
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
    /*********************Detect walls and treasures*/
    //Checks left sensor, add neighbor if no wall and unexplored
    if (isThereAWall(0)==1){ //Left wall present
      grid[currPos]->addWall(Direction((currDir-1)%4), true);
      Serial.println("Leftwall detected");
    } else {
      grid[currPos]->addWall(Direction((currDir-1)%4), false);
      grid[currPos]->addNeighbor(grid[grid[currPos]->neighborCoord(currDir, 0, currPos)]); //Calls neighborCoord first to determine coord of adjacent node,
                                                                                           //then add to neighbor array
    }

    //Checks right sensor, add neighbor if no wall and unexplored
    if (isThereAWall(2)==1){ //Right wall present
      grid[currPos]->addWall(Direction((currDir+1)%4), true);
      Serial.println("Right wall detected");
    } else{ //No wall
      grid[currPos]->addWall(Direction((currDir+1)%4), false);
      grid[currPos]->addNeighbor(grid[grid[currPos]->neighborCoord(currDir, 1, currPos)]);
    }
    
     //Checks front sensor, add neighbor if no wall and unexplored
    if (isThereAWall(1)==1){ //Front wall present
      grid[currPos]->addWall(Direction(currDir), true);
      Serial.println("Front wall detected");
    } else{ //No wall
      grid[currPos]->addWall(Direction(currDir), false);
      grid[currPos]->addNeighbor(grid[grid[currPos]->neighborCoord(currDir, 2, currPos)]);
    }

    /*********************Turns toward next node, update DFS structure*/
    nextNode = explorerPtr->nextNode();
    nextPos  = nextNode->getCoord();
    if (currPos == 2) {
      leftWheel.write(92);
      rightWheel.write(90);
      Serial.println("is done!");
      while(1);
    }
    Serial.print("nextPos:");
    Serial.println(int(nextPos));
    //nextDir = if currDir is north and u turn left then = west, right = east, backward = south
    if(getTurn() == 1) {
      //turn in specified direction
      Serial.println("Left turn");
      leftTurn(); 
      Serial.println("");
    } else if (getTurn() == 2) {
      Serial.println("U-turn");
      leftWheel.write(92);
      rightWheel.write(90);
      Serial.println("is done!");
      Serial.println("");
      //leftTurnFast();
      //leftTurnFast();
      
    } else if (getTurn() == 3) {
      Serial.println("right turn");
      Serial.println("");
      rightTurn();
    }
  
    explorerPtr->travelTo(grid[nextPos]);
    currPos = nextPos;
    //currDir = nextDir;
    //Robot should be following the line toward this next node now
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
  //Serial.print(lineLeft);
  //Serial.print("  ");
  //Serial.print(lineMidLeft);
  //Serial.print("  ");
  //Serial.print(lineMidRight);
  //Serial.print("  ");
  //Serial.print(lineRight);
  //Serial.print("  ");
  //Serial.print(turn);
  //Serial.print("  ");
  //Serial.println();
}

//Return if there is a wall 
int isThereAWall (int sensor){
  if (sensor == 0){  //Y0 - left wall
      digitalWrite(mux_S0, LOW);
      digitalWrite(mux_S1, LOW);
      Serial.print("Left wall value:");
      Serial.println(analogRead(distanceInput));
      return (analogRead(distanceInput) > 180);
  }
  else if (sensor == 1){ //Y1 - front wall
      digitalWrite(mux_S0, HIGH);
      digitalWrite(mux_S1, LOW);
      Serial.print("Front wall value:");
      Serial.println(analogRead(distanceInput));
      return (analogRead(distanceInput) > 200);
  }
  else if (sensor == 2){ //Y2 - right wall
      digitalWrite(mux_S0, LOW);
      digitalWrite(mux_S1, HIGH);
      Serial.print("Right wall value:");
      Serial.println(analogRead(distanceInput));
      return (analogRead(distanceInput) > 180);
  }
  
  //Serial.println("distance");
  //Serial.println(analogRead(distanceInput));
  return 0;
}

//0000 north south east west - front back right left
//Return the most significant 4 bits of wall status
uint8_t wall (){
  uint8_t wallbits = 0;
  //left at bit 4, front at bit 7, right at bit 5
  wallbits |= isThereAWall(0) << 4; //left
  delay(10); //possible delay
  wallbits |= isThereAWall(1) << 7; //front
  delay(10); //possible delay
  wallbits |= isThereAWall(2) << 5; //right
  delay(10); //possible delay
  return wallbits;
}

// Complete 90 degree left turn at intersection
void leftTurn(){
  bool turnDone = false;
  currDir = (currDir-1)%4;
  //walkForward();
  //delay(200);
  bool flag = false;
  while (! turnDone) {
    leftTurnSlow(); //Begin turning left
    delay(100);     
    //Update needed sensor values
    lineMidLeft = analogRead(lineMidLeftPin);
    
    //Test necessary conditions for completion of left turn
    if (!flag && lineMidLeft < blackDetect) flag = true;
    if (flag && lineMidLeft  > blackDetect) turnDone = true;
    
  }
}

// Complete 90 degree right turn at intersection
void rightTurn(){
  bool turnDone = false;
  //walkForward();
  //delay(200);
  bool flag = false;
  currDir = (currDir+1)%4;
  while (! turnDone) {
    rightTurnFast(); ///Begin turning right
    delay(100);
    //Update needed sensor values
    lineMidRight = analogRead(lineMidRightPin);
    printSensors();
    //Test necessary conditions for completion of right turn
    if (!flag && lineMidRight < blackDetect) flag = true;
    if (flag && lineMidRight  > blackDetect) turnDone = true;
    
  }
}

void keepStraight(){
  char turnFlag = 1;
  while (turnFlag) {
    walkForward();
    lineLeft = analogRead(lineLeftPin); lineRight = analogRead(lineRightPin); 
    if (lineLeft < blackDetect && lineRight < blackDetect) turnFlag = 0;
  }
}

void walkForward(){ //Moving forward full speed
  leftWheel.write(94);  //137-92 = 45
  rightWheel.write(88);
}

void leftTurnSlow(){ //Slow turn: turn left with just one wheel
  leftWheel.write(92); //Stop left wheel moving
  rightWheel.write(0); //Move right wheel forward
}

void rightTurnSlow(){ //Slow turn: turn right with one wheel
  leftWheel.write(180); //Move left wheel forward
  rightWheel.write(90); //Stop right wheel moving
}

void leftTurnFast(){ //Fast turn: turn left with both wheels
  leftWheel.write(0);   //Move left wheel backward
  rightWheel.write(0);  //Move right wheel forward
  //currDir = (currDir-1)%4;
}

void rightTurnFast(){ //Fast turn: turn right with both wheels
  leftWheel.write(180); //Move left wheel forward
  rightWheel.write(180);//Move right wheel backward
  //currDir = (currDir+1)%4;
}

void leftDrift(){
  leftWheel.write(90);
  rightWheel.write(0);
}

void rightDrift(){
  leftWheel.write(180); //
  rightWheel.write(90); // 
}

int getTurn() {
  int diff = currPos-nextPos;

  if(diff == -4) nextDir = south;
  else if (diff == 4) nextDir = north;
  else if (diff == 1) nextDir = west;
  else nextDir = east;

  return (currDir - nextDir)%4;
}
