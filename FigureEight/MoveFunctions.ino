//------------------------------------------------------------------------------
// Turn Functions
//------------------------------------------------------------------------------

// Complete 90 degree left turn at intersection
void leftTurn(){
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
}

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
    printSensors();

    // Test if turn is complete
    if (!flag && lineMidRight < blackDetect) flag = true;
    if (flag && lineMidRight  > blackDetect) turnDone = true;
  }
}

void keepStraight(){
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
