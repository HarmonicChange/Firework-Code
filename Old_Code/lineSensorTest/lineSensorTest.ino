int lineMidLeft, lineMidRight, lineRight, lineLeft; //Line Sensor Values Variables
int lineMidLeftPin, lineMidRightPin, lineRightPin, lineLeftPin; //Analog pins with line sensors
int toleranceForward = 100;
int blackDetect = 850; //Under this value means robot is on top of whiteish, above this value means is blackish

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  lineMidLeftPin = A1;
  lineMidRightPin = A2;
  lineRightPin = A3;
  lineLeftPin = A0;
}

void loop() {
  lineMidLeft = analogRead(lineMidLeftPin); lineMidRight = analogRead(lineMidRightPin);
  lineLeft = analogRead(lineLeftPin); lineRight = analogRead(lineRightPin);

  Serial.print("mid left: ");
  Serial.println(lineMidLeft);
  Serial.print("mid right: ");
  Serial.println(lineMidRight);
  Serial.print("far left: ");
  Serial.println(lineLeft);
  Serial.print("far right: ");
  Serial.println(lineRight);

  delay(500);
}
