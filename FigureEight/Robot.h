Servo leftWheel, rightWheel;
//Functions for robot to walk and turn
void walkForward(){ //Moving forward full speed
  leftWheel.write(0);
  rightWheel.write(180);
}

void walkLeft(){ //Slow turn: turn left on just one wheel
  leftWheel.write(90); //Stop left wheel moving
  rightWheel.write(180); //Move right wheel forward
}

void walkRight(){ //Slow turn: turn right on one wheel
  leftWheel.write(0); //Move left wheel forward
  rightWheel.write(90); //Stop right wheel moving
}

void turnLeft(){ //Fast turn: turn left on both wheels
  leftWheel.write(180);   //Move left wheel backward
  rightWheel.write(180);  //Move right wheel forward
}

void turnRight(){ //Fast turn: turn right on both wheels
  leftWheel.write(0); //Move left wheel forward
  rightWheel.write(0);//Move right wheel backward
}
