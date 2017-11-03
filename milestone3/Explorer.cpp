#include "Explorer.h"
//#include "Node.h"



//Explorer::Explorer(Node* startNode) {
//  root = startNode;
//  current = startNode;
//}

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
