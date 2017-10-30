#include "Node.h"

class Explorer {

  Node* root;
  Node* current;

  public:
    Explorer();
    Node* nextNode(); //essentially does the DFS
    Node* travelTo(); //call this when robot has moved
  
};

