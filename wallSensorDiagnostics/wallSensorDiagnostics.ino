int mux_S0 = 7, mux_S1 = 8;
int distanceInput;

int isThereAWall(int);

void setup() {
  Serial.begin(9600);
  pinMode(mux_S0, OUTPUT);
  pinMode(mux_S1, OUTPUT);
  distanceInput = A4;
}

void loop() {
  if (isThereAWall(0) == 1) Serial.println("Leftwall detected");
  if (isThereAWall(1) == 1) Serial.println("Front wall detected");
  if (isThereAWall(2) == 1) Serial.println("Right wall detected");
  Serial.println();
  delay(2000);
}

int isThereAWall (int sensor){
  if (sensor == 0){  //Y0 - left wall
      digitalWrite(mux_S0, LOW);
      digitalWrite(mux_S1, LOW);
      Serial.print("Left wall avg value:");
      int temp;
      for (int i = 0; i < 5; i++){
        temp = temp + analogRead(distanceInput);
        delay(25);
      }
      int wallSense = temp/5;
      Serial.println(wallSense);
      return (wallSense > 250);
  }
  else if (sensor == 1){ //Y1 - front wall
      digitalWrite(mux_S0, HIGH);
      digitalWrite(mux_S1, LOW);
      Serial.print("Front wall avg value:");
      int temp;
      for (int i = 0; i < 5; i++){
        temp = temp + analogRead(distanceInput);
        delay(25);
      }
      int wallSense = temp/5;
      Serial.println(wallSense);
      return (wallSense > 250);
  }
  else if (sensor == 2){ //Y2 - right wall
      digitalWrite(mux_S0, LOW);
      digitalWrite(mux_S1, HIGH);
      Serial.print("Right wall avg value:");
      int temp;
      for (int i = 0; i < 5; i++){
        temp = temp + analogRead(distanceInput);
        delay(25);
      }
      int wallSense = temp/5;
      Serial.println(wallSense);
      return (wallSense > 250);
  }
  
  return 0;
}
