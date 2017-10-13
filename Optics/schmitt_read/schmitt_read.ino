unsigned long lasttime = 0;
unsigned long period = 0;
int LED_7 = 3;
int LED_12 = 4;
int LED_17 = 5;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_7, OUTPUT);
  pinMode(LED_12, OUTPUT);
  pinMode(LED_17, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(2), treasure_ISR, FALLING);
}

void treasure_ISR() {
  period = micros() - lasttime  ;
  lasttime = micros();
}

void loop() {
  //Serial.println(period);
  if(period > 130 && period < 150) {
    digitalWrite(LED_7, HIGH);
    Serial.println("7kHz");  
    period = 0;
  }
  else if(period > 75 && period < 85) {
    digitalWrite(LED_12, HIGH);  
    Serial.println("12kHz");  
    period = 0;
  }
  else if(period > 50 && period < 60) {
    digitalWrite(LED_17, HIGH); 
    Serial.println("17kHz");   
    period = 0;
  }
  else {
    digitalWrite(LED_7, LOW);
    digitalWrite(LED_12, LOW);
    digitalWrite(LED_17, LOW);   
  }
}
