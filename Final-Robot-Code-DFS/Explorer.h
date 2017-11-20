//#ifndef EXPLORER_H
//#define EXPLORER_H
#include "Node.h"
//#include "Node.cpp"

class Explorer {

  Node* root;
  Node* current;

  public:
    Explorer(Node* startNode);    
    Node* nextNode(); //essentially does the DFS
    Node* travelTo(Node*); //call this when robot has moved
    Node* getCurrNode(); //returns current node
    bool isDone();
  
};

Explorer::Explorer(Node* startNode) {
  root = startNode;
  current = startNode;
  startNode->addParent(startNode); //The only node with itself as its own parent is the root node.
}

Node* Explorer::nextNode() {
  Node** neighbors = current->getNeighbors();
  
  for (int i = 0; i<3; i++){
    if (neighbors[i] != NULL && !((*(neighbors+i))->isExplored())){
      return *(neighbors+i);
    }
   }
  
  return current->getParent(); //Reverse traversal to find unexplored neighbors
}

Node* Explorer::travelTo(Node* walkingTo) {
  walkingTo->markAsExplored();
  if (walkingTo == current->getParent()){
    current = walkingTo;
  }
  else{
    walkingTo->addParent(current);
    current = walkingTo;
  }
}

Node* Explorer::getCurrNode(){
  return current;
}

bool Explorer::isDone(){
  if (current==root && (this->nextNode())==current){
    return true;
  }
  else{
    return false;
  }
}



//#endif

