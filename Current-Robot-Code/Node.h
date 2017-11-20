#ifndef NODE_H
#define NODE_H

enum Direction {west=0, north, east, south};

class Node {

  int dir; 
  char coord; //0-19

  //NSEWTTTF: NSEW = walls, TT = treasure, F = flag for done exploring
  char wallTreasures;
  
  Node* neighbors[3]; //pointer to neighboring nodes
  Node* parent; //pointer to parent node
  
  int nextNeighbor;

  public:
    Node(int);
    void addNeighbor(Node*);
    void addWall(Direction, bool);
    bool isExplored();
    void addTreasure(int freq);
    void addParent(Node*);
    void markAsExplored();
    Node** getNeighbors();
    Node* getParent();
    char getCoord();
    char getWallTreasures();
    int neighborCoord(Direction facing, int sensor, int currPos);
};

Node::Node (int pos) {
  coord = pos;
  wallTreasures = 0;
  nextNeighbor = 0;
  for (int i = 0; i<3; i++){
    neighbors[i] = NULL;
  } 
}

void Node::addNeighbor(Node* n) {
  Serial.print("Neighbor:");
  if(n->isExplored()==0) {
    neighbors[nextNeighbor] = n;
   
    Serial.println(int(n->getCoord()));
    nextNeighbor++;
  }
}

Node** Node::getNeighbors() {
  return neighbors;
}

Node* Node::getParent() {
  return parent;
}

char Node::getCoord() {
  return coord;
}

void Node::addWall (Direction d, bool hasWall) {
  if (d == north) {
    if(hasWall) {
      wallTreasures |= (1<<7);
    } else {
      wallTreasures &= ~(1<<7);
    }
  } else if (d == south) {
    if(hasWall) {
      wallTreasures |= (1<<6);
    } else {
      wallTreasures &= ~(1<<6);
    }      
  } else if (d == east) {
    if(hasWall) {
      wallTreasures |= (1<<5);
    } else {
      wallTreasures &= ~(1<<5);
    } 
  } else { //d = west
    if(hasWall) {
      wallTreasures |= (1<<4);
    } else {
      wallTreasures &= ~(1<<4);
    }    
  }
}

bool Node::isExplored () {
  return (wallTreasures & 1);
}

void Node::markAsExplored() {
  wallTreasures |= 1;
}

void Node::addTreasure (int freq) {
  //TODO check how we're passing in freq
  if(freq == 0) {
    wallTreasures |= (1 << 4);
  } else if (freq == 1) {
    wallTreasures |= (1 << 3);
  } else {
    wallTreasures |= (1 << 2);
  }
}

void Node::addParent(Node* par) {
  parent = par;
}

char Node::getWallTreasures() {
  return wallTreasures;
}

/**Returns coord of the neighbor you want to add to the array of neighbors in the current node
*Precondition: there is no wall in between your current node and the adjacent node whose coord is requested.
*Can be made into a private method in Node class that integrates with the addNeighbor function.
*WARNING: hardcoded to only work with 4x5 maze.
*int sensor = 0 means left sensor called this method (no wall on left), = 1 right sensor, = 2 front sensor**/
int Node::neighborCoord(Direction facing, int sensor, int currPos){
  int index;
  
  //Front side has no wall
  if (sensor == 2){ 
    if(facing == north) {
      index = currPos-4;
    } else if(facing == west) {
      index = currPos-1;
    } else if(facing == east) {
      index = currPos+1;
    } else {
      index = currPos+4;
    }
  }
  
  //Right side has no wall
  else if (sensor == 1){ 
    if(facing == north) {
      index = currPos+1;
    } else if(facing == west) {
      index = currPos-4;
    } else if(facing == east) {
      index = currPos+4;
    } else {
      index = currPos-1;
    }
  }

  //Left side has no wall
  else if (sensor == 0){ 
    if(facing == north) {
      index = currPos-1;
    } else if(facing == west) {
      index = currPos+4;
    } else if(facing == east) {
      index = currPos-4;
    } else {
      index = currPos+1;
    }
  }
  return index;
}

#endif
