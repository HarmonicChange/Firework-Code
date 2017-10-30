enum Direction {north, south, west, east};

class Node {

  char coord; //0-19

  //NSEWTTTF: NSEW = walls, TTT = treasure, F = flag for done exploring
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
};
