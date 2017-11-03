#ifndef NODE_H
#define NODE_H

enum Direction {west=0, north, east, south};

class Node {

  int dir; 
  char coord; //0-19

  //NSEWTTTF: NSEW = walls, TT = treasure, F = flag for done exploring
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
    Node** getNeighbors();
    Node* getParent();
    char getCoord();
    char getWallTreasures();
};
#endif
