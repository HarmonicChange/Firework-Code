#include "Node.h"

int currPos;
int currDir;
Node* grid[20]; //4x5 maze representation

void setup() {
  //Set up all pins
  
  //Initial position and direction facing
  currPos = 19;
  currDir = north;
  
  //Initialize all nodes
  for(int i = 0; i < 20; i++) {
    grid[i] = new Node(i);
  }

  //Add wall data we already know - exterior of maze
  //Can be deleted if necessary
  grid[0]->addWall(north, true);
  grid[0]->addWall(west, true);
  grid[1]->addWall(north, true);
  grid[1]->addWall(north, true);
  grid[3]->addWall(north, true);
  grid[3]->addWall(east, true);
  grid[7]->addWall(east, true);
  grid[11]->addWall(east, true);
  grid[15]->addWall(east, true);
  grid[19]->addWall(east, true);
  grid[19]->addWall(south, true);
  grid[18]->addWall(south, true);
  grid[17]->addWall(south, true);
  grid[16]->addWall(south, true);
  grid[16]->addWall(west, true);
  grid[12]->addWall(west, true);
  grid[8]->addWall(west, true);
  grid[4]->addWall(west, true);
}

void loop() {

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
