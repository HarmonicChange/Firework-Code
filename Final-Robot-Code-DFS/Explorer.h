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
    void travelTo(Node*); //call this when robot has moved
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
//  current->printNeighbors();
  //Node* toReturn = NULL; //return the next node to go to
  
  for (int i = 0; i<3; i++){
    
    if (neighbors[i] != NULL && !(neighbors[i]->isExplored())){
//      Serial.print("parent's ");
//      Serial.println(int(current->getParent()->getCoord()));
      
      if(i==(current->nextNeighbor)-1 && current == root) {
        root = neighbors[i];
        root->addParent(root);
        Serial.print("root update ");
        Serial.println(int(root->getCoord()));
        //return root;
      }
      //Serial.println("neighbor+i");
      return *(neighbors+i);
    }
  }
  
  //Serial.println("Parent");
  return current->getParent(); //Reverse traversal to find unexplored neighbors
}

void Explorer::travelTo(Node* walkingTo) {
  walkingTo->markAsExplored();
  //Serial.println("walkingto is markasexplored");
  /*if (walkingTo == current->getParent()){
    current = walkingTo;
  }
  else if (walkingTo != root){
    walkingTo->addParent(current);
    current = walkingTo;
  }*/

  if(walkingTo != root) {
    walkingTo->addParent(current);
  }

  current=walkingTo;
}

Node* Explorer::getCurrNode(){
  return current;
}

bool Explorer::isDone(){
  if (this->nextNode()==current){
    Serial.println("In isDone");
    return true;
  }
  else{
    return false;
  }
}



//#endif

