//#ifndef EXPLORER_H
//#define EXPLORER_H
#include "Node.h"
//#include "Node.cpp"

class Explorer {

  Node* root;
  Node* current;

  public:
    Explorer(Node* startNode) {
    root = startNode;
    current = startNode;
    };
    
    Node* nextNode(); //essentially does the DFS
    Node* travelTo(Node*); //call this when robot has moved
  
};

Node* Explorer::nextNode() {
  Node** neighbors = current->getNeighbors();
  for (int i = 0; i<3; i++){
    if (!(*(neighbors+i))->isExplored()){
      return *(neighbors+i);
    }
  }

  if(current->getParent() == root) {
    return current;
  }
  return current->getParent();
}

Node* Explorer::travelTo(Node* walkingTo) {
  walkingTo->addParent(current);
  current = walkingTo;
}

//#endif

