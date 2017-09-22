// Complete 90 degree left turn at intersection
void leftTurn(){
  bool turnDone = false;
  walkForward();
  delay(200);
  while (! turnDone) {
    leftTurnFast(); //Begin turning left
    delay(100);     
    //Update needed sensor values
    lineMidLeft = analogRead(lineMidLeftPin);
    
    //Test necessary conditions for completion of left turn
    if (lineMidLeft  > blackDetect) turnDone = true;
  }
}

// Complete 90 degree right turn at intersection
void rightTurn(){
  bool turnDone = false;
  walkForward();
  delay(200);
  while (! turnDone) {
    rightTurnFast(); ///Begin turning right
    delay(100);
    //Update needed sensor values
    lineMidRight = analogRead(lineMidRightPin);
    
    //Test necessary conditions for completion of right turn
    if (lineMidRight  > blackDetect) turnDone = true;
  }
}

void keepStraight(){
  char turnFlag = 1;
  while (turnFlag) {
    walkForward();
    lineLeft = analogRead(lineLeftPin); lineRight = analogRead(lineRightPin); 
    if (lineLeft < blackDetect && lineRight < blackDetect) turnFlag = 0;
  }
}

void walkForward(){ //Moving forward full speed
  leftWheel.write(137);  //137-92 = 45
  rightWheel.write(45);
}

void leftTurnSlow(){ //Slow turn: turn left with just one wheel
  leftWheel.write(92); //Stop left wheel moving
  rightWheel.write(0); //Move right wheel forward
}

void rightTurnSlow(){ //Slow turn: turn right with one wheel
  leftWheel.write(180); //Move left wheel forward
  rightWheel.write(90); //Stop right wheel moving
}

void leftTurnFast(){ //Fast turn: turn left with both wheels
  leftWheel.write(0);   //Move left wheel backward
  rightWheel.write(0);  //Move right wheel forward
}

void rightTurnFast(){ //Fast turn: turn right with both wheels
  leftWheel.write(180); //Move left wheel forward
  rightWheel.write(180);//Move right wheel backward
}

void leftDrift(){
  leftWheel.write(90);
  rightWheel.write(0);
}

void rightDrift(){
  leftWheel.write(180); //
  rightWheel.write(90); // 
}




