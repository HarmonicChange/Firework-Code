#include "Node.h"
//#include "Node.cpp"
#include "Explorer.h"
//#include "Explorer.cpp"

int currPos;
Direction currDir;
//Node* grid[20]; //4x5 maze representation

//Array of pointers to nodes
Node* grid[4];

//Look at http://forum.arduino.cc/index.php?topic=37010.0
Explorer* explorerPtr = NULL;

void setup() {

  Serial.begin(4800);
//  //Set up all pins
//  
//  //Initial position and direction facing
//  currPos = 19;
//  currDir = north;
//  
//  //Initialize all nodes
//  for(int i = 0; i < 20; i++) {
//    grid[i] = new Node(i);
//  }
//
//  //Add wall data we already know - exterior of maze
//  //Can be deleted if necessary
//  grid[0]->addWall(north, true);
//  grid[0]->addWall(west, true);
//  grid[1]->addWall(north, true);
//  grid[1]->addWall(north, true);
//  grid[3]->addWall(north, true);
//  grid[3]->addWall(east, true);
//  grid[7]->addWall(east, true);
//  grid[11]->addWall(east, true);
//  grid[15]->addWall(east, true);
//  grid[19]->addWall(east, true);
//  grid[19]->addWall(south, true);
//  grid[18]->addWall(south, true);
//  grid[17]->addWall(south, true);
//  grid[16]->addWall(south, true);
//  grid[16]->addWall(west, true);
//  grid[12]->addWall(west, true);
//  grid[8]->addWall(west, true);
//  grid[4]->addWall(west, true);

  //Initial position and direction facing
  currPos = 3;
  currDir = north;
  int index;
  
  //Initialize all nodes
  for(int i = 0; i < 4; i++) {
   grid[i] = new Node(i);
  }

  //Initialize explorer class
  explorerPtr = new Explorer(grid[currPos]);

  //Add perimeter walls
  grid[0]->addWall(north, true);
  grid[0]->addWall(west, true);
  grid[2]->addWall(west, true);
  grid[2]->addWall(south, true);
  grid[3]->addWall(south, true);
  grid[3]->addWall(east, true);
  grid[1]->addWall(east, true);
  grid[1]->addWall(north, true);

  //Intersection detected
  //Sensor input: left has no wall
  grid[currPos]->addWall(Direction((currDir-1)%4), false);
  if((currDir-1)%4 == north) {
    index = currPos-2;
  } else if((currDir-1)%4 == west) {
    index = currPos-1;
  } else if((currDir-1)%4 == east) {
    index = currPos+1;
  } else {
    index = currPos+2;
  }
  grid[currPos]->addNeighbor(grid[index]); //Add neighbor to the left
  
  //Sensor input: up has no wall
  grid[currPos]->addWall(currDir, false);
  if((currDir)%4 == north) {
    index = currPos-2;
  } else if((currDir)%4 == west) {
    index = currPos-1;
  } else if((currDir)%4 == east) {
    index = currPos+1;
  } else {
    index = currPos+2;
  }
  grid[currPos]->addNeighbor(grid[index]); //Add neighbor to the up
  
  //Sensor input: right has wall
  grid[currPos]->addWall(Direction((currDir+1)%4), true);
  
  //Print node info. Tests getCoord.
  Serial.print("Coordinates:");
  Serial.print(int(grid[currPos]->getCoord()));
  Serial.print(" ");
  Serial.println(grid[currPos]->getWallTreasures(),BIN);
  
  //Get the next node. Tests nextNode method
  Serial.print("Next node:");
  Serial.println(int(explorerPtr->nextNode()->getCoord()));

  //Implement code to navigate to next node. Tests travelTo
  explorerPtr->travelTo(grid[explorerPtr->nextNode()->getCoord()]);
  Serial.print("Traveled to:");
  Serial.println(int(explorerPtr->getCurrNode()->getCoord()));
}

void loop() {
  while (!(explorerPtr->isDone())){
    
  }
  
  //Continue to follow the line until intersection

/*
  //Intersection detected
    //Detect walls and link neighbors
    if(northWallDetector == 1) { //condition for detecting wall
      grid[currPos].addWall(north, true);
    }
    if(westWallDetector == 1) {
      grid[currPos].addWall(west, true);
    }
    if(eastWallDetector == 1) {
      grid[currPos}.addWall(east, true);
    }
    
    //Detect treasure
    if(treasureDetector == 1) {
      //TODO check freq encoding
      grid[currPos].addTreasure(freq);
    }
*/
    
    //Send maze data
    
    
    //Turn toward next neighbor
    
}
