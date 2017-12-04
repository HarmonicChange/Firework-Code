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
// 0 = keep straight, 1 = turn leftin, 2 = turn right
   // void intersect(); 
   // void turn(getTurn()l);

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
  //stopMoving();
  //maze[currPos] = 0;
  if(backTrack == false) {   
    delay(50);
    lookAround();
  }

  nextNode = explorerPtr->nextNode();
  if (nextNode == grid[currPos]->getParent()) backTrack = true;
  else backTrack = false;
  nextPos  = nextNode->getCoord();

  bool sendFailed = true;
  while (sendFailed) {
    Serial.println("Sending");
    sendFailed = transmitData(); // These are set to true IFF send failed
    sendFailed = false;
  }
 
   
}


// Helper Function
// Complete 90 degree left turn at intersection
void leftTurn(){
  bool turnDone = false;
  bool flag = false;
  walkForward();
  delay(150);

  // Begin turning leftret 
  leftTurnFast(); 
  delay(100);  
  
  while (! turnDone) {  

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
  delay(150);

  // Begin turning right
  rightTurnFast(); 
  delay(100);
  
  while (! turnDone) {

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
  /*bool turnDone = false;
  bool flag1 = false;
  bool flag2 = false;
  bool flag3 = false;
  walkForward();
  delay(150);
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
  // Update Direction it's facing
  if (currDir == 0) currDir = 2;
  else if (currDir == 1) currDir = 3;
  else if (currDir == 2) currDir = 0;
  else if (currDir == 3) currDir = 1;*/
  leftTurn();
  bool turnDone = false;
  bool flag = false;

  // Begin turning leftret 
  leftTurnFast(); 
  delay(100);  
  
  while (! turnDone) {  

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
  else if (diff == -1) nextDir = east;
  
  if(currDir - nextDir < 0) return currDir - nextDir + 4;
  return (currDir - nextDir)%4;
}


//------------------------------------------------------------------------------
// Detect Stuff Around
//------------------------------------------------------------------------------

// Look around for walls, update maze
void lookAround(){
  // Check wall to the left
  if (isThereAWall(0)) { 
    grid[currPos]->addWall(Direction((currDir+1)%4), true);
    Serial.println("Left wall detected");
    if (currDir == 3) maze[currPos] += 1;
    else if (currDir == 2) maze[currPos] += 8;
    else if (currDir == 1) maze[currPos] += 4;
    else if (currDir == 0) maze[currPos] += 2;
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
    if (currDir == 3) maze[currPos] += 4;
    else if (currDir == 2) maze[currPos] += 2;
    else if (currDir == 1) maze[currPos] += 1;
    else if (currDir == 0) maze[currPos] += 8;
  } else{ //No wall
    if(currDir - 1 < 0) grid[currPos]->addWall(Direction(currDir+3), false);
    else grid[currPos]->addWall(Direction((currDir-1)%4), false);
    grid[currPos]->addNeighbor(grid[grid[currPos]->neighborCoord(currDir, 1, currPos)]);
  }
    
  // Check wall in front
  if (isThereAWall(1)){
    grid[currPos]->addWall(Direction(currDir), true);
    Serial.println("Front wall detected");
    if (currDir == 3) maze[currPos] += 8;
    else if (currDir == 2) maze[currPos] += 4;
    else if (currDir == 1) maze[currPos] += 2;
    else if (currDir == 0) maze[currPos] += 1;
  } else{ //No wall
    grid[currPos]->addWall(Direction(currDir), false);
    grid[currPos]->addNeighbor(grid[grid[currPos]->neighborCoord(currDir, 2, currPos)]);
  }
}



// Return if there is a wall 
int isThereAWall (int sensor){
  int wallCounter = 0;
  int temp = 0;
  if (sensor == 0){  //Y0 - left walle;
      bool normal = false;
      //int temp = 0;
      //while (!normal){
        digitalWrite(mux_S0, LOW);
        digitalWrite(mux_S1, LOW);
        Serial.print("Left wall avg value:");
        for (int i=0; i<6; i++) {
          temp = temp+analogRead(distanceInput);
          if (analogRead(distanceInput) > 230) wallCounter++;
          delay(25);
        }
        //temp = temp/5;
        //Serial.println(temp);
        //Serial.println(wallCounter); //How many times we dectect wall out of 
        /*if (temp > 500) {
          Serial.println("... Value too big!");
        }
        else {
          normal = true;
        }*/
      //}
  }
  else if (sensor == 1){ //Y1 - front wall
      digitalWrite(mux_S0, HIGH);
      digitalWrite(mux_S1, LOW);
      Serial.print("Front wall avg value:");
      for (int i=0; i<6; i++) {
        temp = temp+analogRead(distanceInput);
        if (analogRead(distanceInput) > 100) wallCounter++;
        delay(25);
      }
      //How many times we dectect wall out of 
      //temp = temp/5;      
      //Serial.println(temp);
      //return (temp > 120); //Dark condition 200, facing the window, sunny day, >130. Recalibrate using wall sensor diagnostic before start
  }
  else if (sensor == 2){ //Y2 - right wall
      digitalWrite(mux_S0, LOW);
      digitalWrite(mux_S1, HIGH);
      Serial.print("Right wall avg value:");
      for (int i=0; i<6; i++) {
        temp = temp+analogRead(distanceInput);
        if (analogRead(distanceInput) > 250) wallCounter++;
        delay(25);
      }
      //temp = temp/5;      
      //Serial.println(temp);
      //return (temp > 250);
  }
  temp = temp/5;
  Serial.println(temp);
  Serial.print("Number of Wall Count:");
  Serial.println(wallCounter); 
  if (wallCounter >= 4) return 1;
  else return 0;
}

//------------------------------------------------------------------------------
// Basic Movements
//------------------------------------------------------------------------------
// All Basic Movements are Helper Functions, except I guess walkForward()


//Moving forward full speed
void stopMoving(){ 
  leftWheel.write(91); //137-92 = 45
  rightWheel.write(90);
}

//Moving forward full speed
void walkForward(){ 
  leftWheel.write(115);  //137-92 = 45
  rightWheel.write(67);
}


//Moving backward full speed
void walkBackward(){ 
  leftWheel.write(67);  //137-92 = 45
  rightWheel.write(115);
}

//Slow turn: turn left with just one wheel
void leftTurnSlow(){ 
  leftWheel.write(91); 
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
bool transmitData() {
  byte robotData = currPos | (currDir << 5);
  byte mazeData = (0b10000000 | maze[currPos]);
  if (currPos==nextPos) mazeData = (0b11000000 | maze[currPos]);
  return sendRF(robotData | (mazeData << 8));
}

// Helper Function
bool sendRF(int sendData){
    // First, stop listening so we can talk.
    radio.stopListening();

    // Send data over RF
    printf("Now sending %x... ",sendData);
    bool ok = radio.write( &sendData, sizeof(int) );
    
    

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
    int got_resp;
    if ( timeout )
    {
      printf("Failed, response timed out.\n\r");
    }
    else
    {
      // Grab the response, compare, and send to debugging spew
      
      radio.read( &got_resp, sizeof(int) );
      printf("Got response %x\n\r",got_resp);
    }

    // Try again 1s later
    delay(1000);
    return (timeout | (got_resp != sendData));
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
  stopMoving();
  lineMidLeftPin = A1;
  lineMidRightPin = A2;
  lineRightPin = A3;
  lineLeftPin = A0;

  distanceInput = A4;
  mic = A5;

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
  Serial.println("Waiting for start flag");
  while (!startFlag){
    stopMoving();
    return_freq();
    if(heard == 1) {
      delay(750);
    }
    else if (heard == 2){
      startFlag = true;
      Serial.println("Tone Detected");
    }
    if (digitalRead(startPin) == HIGH) {
      startFlag = true; 
      Serial.println("Button Detected");
    }
  }
  Serial.println("Starting...");
}

void return_freq() {
  //cli();  // UDRE interrupt slows this way down on arduino1.0
  for (int i = 0 ; i < 256; i += 2) { // save 128 samples
    fft_input[i] = analogRead(mic); // put analog input (pin A0) into even bins
    fft_input[i + 1] = 0; // set odd bins to 0
  }
  fft_window(); // window the data for better frequency response
  fft_reorder(); // reorder the data before doing the fft
  fft_run(); // process the data in the fft
  fft_mag_log(); // take the output of the fft
  //sei();
  //Serial.println(fft_log_out[9]);
  if(fft_log_out[9] > 70){
    heard++;
  };
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

