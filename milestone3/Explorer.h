#ifndef EXPLORER_H
#define EXPLORER_H
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

#endif

