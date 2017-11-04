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
};

Node::Node (int pos) {
  coord = pos;
  wallTreasures = 0;
  nextNeighbor = 0;
}

void Node::addNeighbor(Node* n) {
  if(n->isExplored()) {
    neighbors[nextNeighbor] = n;
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

#endif
