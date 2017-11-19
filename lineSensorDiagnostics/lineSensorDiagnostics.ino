int lineMidLeft, lineMidRight, lineRight, lineLeft;             //Line Sensor Values Variables
int lineMidLeftPin, lineMidRightPin, lineRightPin, lineLeftPin; // Analog pins with line sensors
int blackDetect = 600;      // Threshold above which sensors are reading a black line

void printSensors();

void setup() {
  Serial.begin(9600);

  lineMidLeftPin = A1;
  lineMidRightPin = A2;
  lineRightPin = A3;
  lineLeftPin = A0;
}

void loop() {
  // put your main code here, to run repeatedly:
  printSensors();
}

void printSensors(){
  lineLeft = analogRead(lineLeftPin);
  lineMidLeft = analogRead(lineMidLeftPin);
  lineMidRight = analogRead(lineMidRightPin);
  lineRight = analogRead(lineRightPin);
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
