int SPI_cs_pin = 3;
int SPI_data_pin = 4;
int SPI_clock_pin = 5;

// Robot Info:
char trav[24] = {
  0,  1,  2,  3,
  7,  6,  5,  4,
  8, 12,  8,  4,
  5,  9, 13, 17,
 16, 17, 18, 19,
 15, 11, 10, 14, 
};
char maze[20] = {
  0b000111,  0b000011,  0b000011,  0b111001,
  0b000101,  0b000001,  0b000011,  0b001010,
  0b101100,  0b001100,  0b010101,  0b001001,
  0b001110,  0b001100,  0b000100,  0b001000,
  0b000111,  0b000010,  0b000010,  0b001010,

};

//Create our SPI function to send 8 bit of data (MSB first)
void sendSPI(byte data){
  digitalWrite(SPI_cs_pin, LOW);
  digitalWrite(SPI_cs_pin, HIGH); //Create rising edge to start sending packages

  //Send a byte of data, bit by bit, MSB first
  for (int loop = 0; loop < 8; loop++){ 
    
    //If bit is high, set data pin high; if bit is low, set data pin low 
    if ( !!(data & (1 << (7-loop))) ) digitalWrite(SPI_data_pin, HIGH);  
    else digitalWrite(SPI_data_pin, LOW);
    
    //Create rising edge of clock to indicate data is ready
    digitalWrite(SPI_clock_pin, LOW);
    digitalWrite(SPI_clock_pin, HIGH);
  }
  digitalWrite(SPI_cs_pin, LOW); //Create falling edge to indicate stop sending packages
  digitalWrite(SPI_clock_pin, LOW);
  delay(500);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(SPI_cs_pin, OUTPUT);
  pinMode(SPI_data_pin, OUTPUT);
  pinMode(SPI_clock_pin, OUTPUT);
  digitalWrite(SPI_cs_pin, LOW);      //Initialize cs to be LOW
  digitalWrite(SPI_clock_pin, LOW);   //Initialize clock to be LOW
}

void loop() {
  // put your main code here, to run repeatedly:
    for(int i = 0; i<46; i++){
      if (i < 24) {
        sendSPI(trav[i]);
        sendSPI(0b10000000 | maze[trav[i]]);     
      }
      else {
        sendSPI(trav[23 - (i-23)]);
        sendSPI(0b10000000 | maze[trav[23 - (i-23)]]); 
      }
    }
}
