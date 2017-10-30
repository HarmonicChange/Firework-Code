#include "Node.h"

Node::Node (int pos) {
  coord = pos;
  wallTreasures = 0;
  nextNeighbor = 0;
}

void Node::addNeighbor(Node* n) {
  neighbors[nextNeighbor] = n;
  nextNeighbor++;
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
    wallTreasure |= (1 << 4);
  } else if (freq == 1) {
    wallTreasure |= (1 << 3);
  } else {
    wallTreasure |= (1 << 2);
  }
}

void Node::addParent(Node* par) {
  parent = par;
}

