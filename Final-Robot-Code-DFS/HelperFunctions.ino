//-----------------------------------------------------------------------------
// Functions to call on
//-----------------------------------------------------------------------------

// These are the functions you should call on in the main program:

// Call in the initial setup:
   // void initializeStuff();        // Initializes variables, motors, etc
   // void initializeRFStuff();      // Initializes RF stuff
   // void waitForStart();           // Freezes until button is pressed

// Call when you reach an intersection
// It sends all data over RF, updates variables, then continues
// 0 = keep straight, 1 = turn left, 2 = turn right
   // void intersect(); 
   // void turn(int dir);

// Call as needed
   // void updateLineSensors();      // Updates sensor values (mostly used as helper)
   // void printSensors();           // Print sensor values, for debugging


//------------------------------------------------------------------------------
// Turn Functions
//------------------------------------------------------------------------------

// Helper function, do not call directly
void updateRobotLocation(){
  if (currDir == 2) currPos += 4;
  else if (currDir == 3) currPos += 1;
  else if (currDir == 0) currPos -= 4;
  else if (currDir == 1) currPos -= 1;
}

// Call when you reach an intersection
// Sends all data, updates variables, then acts as appropriate
// Make sure to uncomment the while loop once radio is implemented
void intersect(){
  leftWheel.write(92); 
  rightWheel.write(90); 
  lookAround();
  //updateRobotLocation(); 
  bool sendFailed = true;
//  while (sendFailed) {
//    bool s1 = transmitRobot(); // These are set to true IFF send failed
//    bool s2 = transmitMaze();
//    sendFailed = s1 | s2; 
//  }
   
}


// Helper Function
// Complete 90 degree left turn at intersection
void leftTurn(){
  bool turnDone = false;
  bool flag = false;
  //walkForward();
  //delay(200);
  
  while (! turnDone) {
    // Begin turning left
    leftTurnFast(); 
    delay(100);     

    // Update needed sensor values
    updateLineSensors();
    //printSensors();
    
    // Test if turn is complete
    if (!flag && lineMidLeft < blackDetect) flag = true;
    if (flag && lineMidLeft  > blackDetect) turnDone = true;
  }

  // Update Direction it's facing
  if (currDir == 0) currDir = 1;
  else if (currDir == 1) currDir = 2;
  else if (currDir == 2) currDir = 3;
  else if (currDir == 3) currDir = 0;
  
}

// Helper Function
// Complete 90 degree right turn at intersection
void rightTurn(){
  bool turnDone = false;
  bool flag = false;
  walkForward();
  delay(200);
  
  while (! turnDone) {
    // Begin turning right
    rightTurnFast(); 
    delay(100);

    // Update needed sensor values
    updateLineSensors();
    //printSensors();

    // Test if turn is complete
    if (!flag && lineMidRight < blackDetect) flag = true;
    if (flag && lineMidRight  > blackDetect) turnDone = true;
  }
    // Update Direction it's facing
  if (currDir == 0) currDir = 3;
  else if (currDir == 1) currDir = 0;
  else if (currDir == 2) currDir = 1;
  else if (currDir == 3) currDir = 2;
}

// Helper Function
void keepStraight(){
  bool flag = true;
  while (flag) {
    walkForward();
    lineLeft = analogRead(lineLeftPin); lineRight = analogRead(lineRightPin); 
    if (lineLeft < blackDetect && lineRight < blackDetect) flag = false;
  }
}

void UTurn(){
  bool turnDone = false;
  bool flag1 = false;
  bool flag2 = false;
  bool flag3 = false;
  if (currDir-2 <0) currDir=currDir -2+4;
  else currDir = (currDir-2)%4;
  //walkForward();
  //delay(200);
  bool flag = false;
  while (! turnDone) {
    leftTurnFast(); //Begin turning left
    delay(100);     
    //Update needed sensor values
    lineMidLeft = analogRead(lineMidLeftPin);
    
    //Test necessary conditions for completion of left turn
    if (!flag1 && lineMidLeft < blackDetect) flag1 = true;
    else if (!flag2 && flag1 && lineMidLeft > blackDetect) flag2 = true;
    else if (!flag3 && flag2 && flag1 && lineMidLeft < blackDetect) flag3 = true;
    else if (flag3 && flag2 && flag1 && lineMidLeft > blackDetect) turnDone = true;
    
  }
  
}

void turn(int dir){
  if (dir == 0) {keepStraight(); Serial.println("straight");}
  else if (dir == 3) {leftTurn(); Serial.println("left");}
  else if (dir == 1) {rightTurn(); Serial.println("right");}
  else if (dir == 2) {UTurn(); Serial.println("uturn");}
  //else printf("You can't turn that way!");
}

int getTurn() {
  int diff = currPos-nextPos;

  if (diff == -4) nextDir = south;        // Want to south
  else if (diff == 4) nextDir = north;
  else if (diff == 1) nextDir = west;
  else nextDir = east;
  if(currDir - nextDir < 0) return currDir - nextDir + 4;
  return (currDir - nextDir)%4;
}


//------------------------------------------------------------------------------
// Detect Stuff Around
//------------------------------------------------------------------------------

// Look around for walls, update maze
void lookAround (){
  // Check wall to the left
  if (isThereAWall(0)) { 
    grid[currPos]->addWall(Direction((currDir+1)%4), true);
    Serial.println("Left wall detected");
    if (currDir == 3) maze[currPos] += 1;
    else maze[currPos] += pow(2, currDir + 1);
  } 
  else {
    grid[currPos]->addWall(Direction((currDir+1)%4), false);
    grid[currPos]->addNeighbor(grid[grid[currPos]->neighborCoord(currDir, 0, currPos)]); //Calls neighborCoord first to determine coord of adjacent node,
  }

  // Check wall to the right
  if (isThereAWall(2)){
    if(currDir - 1 < 0) grid[currPos]->addWall(Direction(currDir+3), true);
    else grid[currPos]->addWall(Direction((currDir-1)%4), true);
    Serial.println("Right wall detected");
    if (currDir == 0) maze[currPos] += 8;
    else maze[currPos] += pow(2, currDir - 1);
  } else{ //No wall
    if(currDir - 1 < 0) grid[currPos]->addWall(Direction(currDir+3), false);
    else grid[currPos]->addWall(Direction((currDir-1)%4), false);
    grid[currPos]->addNeighbor(grid[grid[currPos]->neighborCoord(currDir, 1, currPos)]);
  }
    
  // Check wall in front
  if (isThereAWall(1)){
    grid[currPos]->addWall(Direction(currDir), true);
    Serial.println("Front wall detected");
    maze[currPos] += pow(2, currDir);
  } else{ //No wall
    grid[currPos]->addWall(Direction(currDir), false);
    grid[currPos]->addNeighbor(grid[grid[currPos]->neighborCoord(currDir, 2, currPos)]);
  }
}



// Return if there is a wall 
int isThereAWall (int sensor){
  if (sensor == 0){  //Y0 - left wall
      digitalWrite(mux_S0, LOW);
      digitalWrite(mux_S1, LOW);
      Serial.print("Left wall avg value:");
      int temp = 0;
      for (int i=0; i<5; i++) {
        temp = temp+analogRead(distanceInput);
        delay(25);
      }
      temp = temp/5;
      Serial.println(temp);
      return (temp > 150);
  }
  else if (sensor == 1){ //Y1 - front wall
      digitalWrite(mux_S0, HIGH);
      digitalWrite(mux_S1, LOW);
      Serial.print("Front wall avg value:");
      int temp = 0;
      for (int i=0; i<5; i++) {
        temp = temp+analogRead(distanceInput);
        delay(25);
      }
      temp = temp/5;      
      Serial.println(temp);
      return (temp > 200); //Dark condition 200, facing the window, sunny day, >130. Recalibrate using wall sensor diagnostic before start
  }
  else if (sensor == 2){ //Y2 - right wall
      digitalWrite(mux_S0, LOW);
      digitalWrite(mux_S1, HIGH);
      Serial.print("Right wall avg value:");
      int temp = 0;
      for (int i=0; i<5; i++) {
        temp = temp+analogRead(distanceInput);
        delay(25);
      }
      temp = temp/5;      
      Serial.println(temp);
      return (temp > 100);
  }
  
  return 0;
}

//------------------------------------------------------------------------------
// Basic Movements
//------------------------------------------------------------------------------
// All Basic Movements are Helper Functions, except I guess walkForward()

//Moving forward full speed
void walkForward(){ 
  leftWheel.write(115);  //137-92 = 45
  rightWheel.write(67);
}

//Slow turn: turn left with just one wheel
void leftTurnSlow(){ 
  leftWheel.write(92); 
  rightWheel.write(0); 
}

//Slow turn: turn right with one wheel
void rightTurnSlow(){ 
  leftWheel.write(180);
  rightWheel.write(90); }

//Fast turn: turn left with both wheels
void leftTurnFast(){ 
  leftWheel.write(0);   
  rightWheel.write(0);  
}

//Fast turn: turn right with both wheels
void rightTurnFast(){ 
  leftWheel.write(180); 
  rightWheel.write(180);
}

//Drift: drift to the left 
void leftDrift(){
  leftWheel.write(90);
  rightWheel.write(0);
}

//Drift: drift to the right
void rightDrift(){
  leftWheel.write(180); //
  rightWheel.write(90); // 
}

//------------------------------------------------------------------------------
// Radio Transmission Functions
//------------------------------------------------------------------------------

// Call this in setup, after initializeStuff()
void initializeRFStuff(){
  // Include at the start:
    //#include <SPI.h>
    //#include "nRF24L01.h"
    //#include "RF24.h"
    //#include "printf.h"
    // Hardware config for transmitter:
    //RF24 radio(9,10);  // Set up nRF24L01 radio on SPI bus plus pins 9 & 10
    //const uint64_t pipes[2] = { 0x000000002ALL, 0x000000002BLL };  // Radio pipe addresses for the 2 nodes to communicate.

  // Setup RF radio transmission
  printf_begin();
  printf("ROLE: Transmitter");
  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
  radio.setAutoAck(true);
  // set the channel
  radio.setChannel(0x50);
  // set the power
  // RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
  radio.setPALevel(RF24_PA_HIGH);
  //RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);

  radio.startListening();
  radio.printDetails();
}

//  Helper Function
bool transmitRobot() {
  return sendRF(currPos);
}

// Helper Function
bool transmitMaze() {
  return sendRF(0b10000000 | maze[currPos]);
}

// Helper Function
bool sendRF(byte sendData){
    // First, stop listening so we can talk.
    radio.stopListening();

    // Send data over RF
    printf("Now sending %x... ",sendData);
    bool ok = radio.write( &sendData, sizeof(byte) );
    
    

    if (ok)
      printf("ok...");
    else
      printf("failed.\n\r");

    // Now, continue listening
    radio.startListening();

    // Wait here until we get a response, or timeout (250ms)
    unsigned long started_waiting_at = millis();
    bool timeout = false;
    while ( ! radio.available() && ! timeout )
      if (millis() - started_waiting_at > 200 )
        timeout = true;

    // Describe the results
    if ( timeout )
    {
      printf("Failed, response timed out.\n\r");
    }
    else
    {
      // Grab the response, compare, and send to debugging spew
      byte got_resp;
      radio.read( &got_resp, sizeof(byte) );
      printf("Got response %x\n\r",got_resp);
    }

    // Try again 1s later
    delay(1000);
    return timeout;
}

//------------------------------------------------------------------------------
// Other Functions
//------------------------------------------------------------------------------

/**
 * Initializes serial monitor (9600), pin connections, pinmodes, and Node/Explorer structures
 */
void initializeStuff(){
  // Setup Serial and pin directions
  Serial.begin(9600);
  pinMode(startPin, INPUT);
  pinMode(mux_S0, OUTPUT);
  pinMode(mux_S1, OUTPUT);

  // Initialize system variables
  leftWheel.attach(5);
  rightWheel.attach(6);
  //Stop both wheels at the beginning
  leftWheel.write(92);  //Stop left wheel moving
  rightWheel.write(90); //Stop right wheel forward
  lineMidLeftPin = A1;
  lineMidRightPin = A2;
  lineRightPin = A3;
  lineLeftPin = A0;

  distanceInput = A4;
  // IRInput = A5?

  //Initialize all nodes
  for(int i = 0; i < 20; i++) {
    grid[i] = new Node(i);
  }
  //Initialize explorer
  explorerPtr = new Explorer(grid[19]);


}

// Call this at the end of setup(), it locks the program until the button is pressed
//TODO should implement tone start here
void waitForStart(){
  bool startFlag = false;
  Serial.println("Waiting for button press");
  while (!startFlag){
    if (digitalRead(startPin) == HIGH) startFlag = true; 
    leftWheel.write(92);  //Stop left wheel moving
    rightWheel.write(90); //Stop right wheel forward
  }
  Serial.println("Starting...");
}

// Call as needed
void updateLineSensors(){
  lineMidLeft = analogRead(lineMidLeftPin); 
  lineMidRight = analogRead(lineMidRightPin);
  lineLeft = analogRead(lineLeftPin); 
  lineRight = analogRead(lineRightPin);
}

// Call as needed
void printSensors(){
  // Used for debugging
  // Prints line sensor values left to right
  Serial.print(lineLeft);
  Serial.print("  ");
  Serial.print(lineMidLeft);
  Serial.print("  ");
  Serial.print(lineMidRight);
  Serial.print("  ");
  Serial.print(lineRight);
  Serial.print("  ");
  Serial.println();
}

