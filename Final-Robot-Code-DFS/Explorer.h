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
  Node** neighbors = current->getNeighbors(); //list of current neighbors
  Node* toReturn = NULL; //return the next node to go to
  
  for (int i = 0; i<3; i++){
    if (neighbors[i] != NULL && !(neighbors[i]->isExplored())){
      toReturn = *(neighbors+i);
      if(i==(current->nextNeighbor)-1 && current == root) {
        root = toReturn;
        Serial.println("root update");
      }
    }
  }

  if(toReturn != NULL) {
    return toReturn;
  } else {
    return current->getParent(); //Reverse traversal to find unexplored neighbors
  }
}

Node* Explorer::travelTo(Node* walkingTo) {
  current = walkingTo;
  current->markAsExplored();
//  walkingTo->markAsExplored();
//  
//  if (walkingTo == current->getParent()){
//    current = walkingTo;
//  }
//  else{
//    walkingTo->addParent(current);
//    current = walkingTo;
//  }
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

