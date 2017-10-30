#include "Explorer.h"

Explorer::Explorer(Node* startNode) {
  root = startNode;
  current = startNode;
}

Node* Explorer::nextNode() {
  
}

Node* Explorer::travelTo(Node* walkingTo) {
  current = walkingTo;
}

