void updateRobotLocation(){
  if (robot_dir == 0) robot_loc += 4;
  else if (robot_dir == 1) robot_loc += 1;
  else if (robot_dir == 2) robot_loc -= 4;
  else if (robot_dir == 3) robot_loc -= 1;
}

//------------------------------------------------------------------------------
// Turn Functions
//------------------------------------------------------------------------------

// Complete 90 degree left turn at intersection
void leftTurn(){
  updateRobotLocation();
  bool turnDone = false;
  bool flag = false;
  walkForward();
  delay(200);
  
  while (! turnDone) {
    // Begin turning left
    leftTurnFast(); 
    delay(100);     

    // Update needed sensor values
    updateLineSensors();
    printSensors();
    
    // Test if turn is complete
    if (!flag && lineMidLeft < blackDetect) flag = true;
    if (flag && lineMidLeft  > blackDetect) turnDone = true;
  }

  // Update Direction it's facing
  if (robot_dir <= 2) robot_dir += 1;
  else robot_dir = 0; 
}

// Complete 90 degree right turn at intersection
void rightTurn(){
  updateRobotLocation();
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
    printSensors();

    // Test if turn is complete
    if (!flag && lineMidRight < blackDetect) flag = true;
    if (flag && lineMidRight  > blackDetect) turnDone = true;
  }
  if (robot_dir > 0) robot_dir -= 1;
  else robot_dir = 3; 
}

void keepStraight(){
  updateRobotLocation();
  bool flag = true;
  while (flag) {
    walkForward();
    lineLeft = analogRead(lineLeftPin); lineRight = analogRead(lineRightPin); 
    if (lineLeft < blackDetect && lineRight < blackDetect) flag = false;
  }
}

//------------------------------------------------------------------------------
// Basic Movements
//------------------------------------------------------------------------------

//Moving forward full speed
void walkForward(){ 
  leftWheel.write(137);  //137-92 = 45
  rightWheel.write(45);
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
void initializeRFStuff(){
  // Print RF Preamble
  printf_begin();
  printf("\n\rRF24/examples/GettingStarted/\n\r");
  printf("ROLE: %s\n\r",role_friendly_name[role]);
  printf("*** PRESS 'T' to begin transmitting to the other node\n\r");

  // Setup radio transmission
  radio.begin();
  radio.setRetries(15,15); // optionally, increase the delay between retries & # of retries
  radio.setAutoAck(true);
  radio.setChannel(0x50); // Set Channel
  radio.setPALevel(RF24_PA_MAX); // Set Power, // RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
  radio.setDataRate(RF24_250KBPS); // Set Rate, //RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  //radio.setPayloadSize(8); // optionally, reduce the payload size. Seems to improve reliability
  // Open pipes to other nodes for communication
  if ( role == role_ping_out ) {
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1,pipes[1]); }
  else {
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]); }
  radio.startListening(); // Start Listening
  radio.printDetails(); // Dump the configuration of the rf unit for debugging
}

byte joinRobot(){
  return (robot_dir << 5) + (robot_loc);
}

void transmitRobot() {
  radio.stopListening();
  bool ok = radio.write( joinRobot(), sizeof(byte));
  printf("Now sending robot data...");         
  if (ok) printf("ok...");
  else printf("failed :( \n\r");

  // Data sent, start listening
  radio.startListening();
  // Wait here until we get a response, or timeout (250ms)
  unsigned long started_waiting_at = millis();
  bool timeout = false;
  while ( ! radio.available() && ! timeout )
    if (millis() - started_waiting_at > 200 )
      timeout = true;

  // Describe the results
  if ( timeout ) printf("Failed, response timed out.\n\r");
  else
  {
    // Grab the response, compare, and send to debugging spew
    unsigned long got_time;
    radio.read( &got_time, sizeof(unsigned long) );
  
    // Spew it
    printf("Got response %lu, round-trip delay: %lu\n\r",got_time,millis()-got_time);
  }
  

}

void transmitMaze() {
  // First, stop listening so we can talk.
  radio.stopListening();

  // Send maze array        
  bool ok = radio.write( &maze, 25*sizeof(unsigned char));
  printf("Now sending current maze...");         
  if (ok) printf("ok...");
  else printf("failed :( \n\r");
  
  // Now, continue listening
  radio.startListening();
  
  // Wait here until we get a response, or timeout (250ms)
  unsigned long started_waiting_at = millis();
  bool timeout = false;
  while ( ! radio.available() && ! timeout )
    if (millis() - started_waiting_at > 200 )
      timeout = true;

  // Describe the results
  if ( timeout ) printf("Failed, response timed out.\n\r");
  else
  {
    // Grab the response, compare, and send to debugging spew
    unsigned long got_time;
    radio.read( &got_time, sizeof(unsigned long) );
  
    // Spew it
  	printf("Got response %lu, round-trip delay: %lu\n\r",got_time,millis()-got_time);
  }
 
}

//------------------------------------------------------------------------------
// Other Functions
//------------------------------------------------------------------------------

void initializeStuff(){
  // Setup Serial and pin directions
  Serial.begin(9600);
  pinMode(startPin, INPUT);

  // Initialize system variables
  leftWheel.attach(5);
  rightWheel.attach(3);
  lineMidLeftPin = A1;
  lineMidRightPin = A2;
  lineRightPin = A3;
  lineLeftPin = A0;

  // RF transmission Data
  maze[5][4] = 
  {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,  
  };
  robot_loc = 0;
  robot_dir = 0;
}

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

void updateLineSensors(){
  lineMidLeft = analogRead(lineMidLeftPin); 
  lineMidRight = analogRead(lineMidRightPin);
  lineLeft = analogRead(lineLeftPin); 
  lineRight = analogRead(lineRightPin);
}

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
  Serial.print(turn);
  Serial.print("  ");
  Serial.println();
}
