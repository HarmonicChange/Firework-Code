unsigned long lasttime = 0;
unsigned long period = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(2), treasure_ISR, FALLING);
}

void treasure_ISR() {
  period = micros() - lasttime  ;
  lasttime = micros();
}

void loop() {
  //Serial.println(period);
  if(period > 130 && period < 150) {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("7kHz");  
    period = 0;
  }
  else if(period > 75 && period < 85) {
    digitalWrite(LED_BUILTIN, HIGH);  
    Serial.println("12kHz");  
    period = 0;
  }
  else if(period > 50 && period < 60) {
    digitalWrite(LED_BUILTIN, HIGH); 
    Serial.println("17kHz");   
    period = 0;
  }
  else {
    digitalWrite(LED_BUILTIN, LOW);   
  }
  delay(500); 
}
