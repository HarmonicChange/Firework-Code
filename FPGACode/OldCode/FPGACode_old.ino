q// SPI Settings
#include <SPI.h>
SPISettings settingsB(1000000, MSBFIRST, SPI_MODE0); //Transfer SPI data to FPGAat Clock rising edge
int SSpin = 10;

// Radio Settings
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
  // Hardware configuration
RF24 radio(9,10);                                                         // Set up nRF24L01 radio on SPI bus plus pins 9 & 10
  // Topology
const uint64_t pipes[2] = { 0x000000002ALL, 0x000000002BLL };             // Radio pipe addresses for the 2 nodes to communicate.
  // Role management
typedef enum { role_ping_out = 1, role_pong_back } role_e;                // The various roles supported by this sketch
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"}; // The debug-friendly names of those roles
role_e role = role_pong_back;                                             // The role of the current running sketch

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

void setup() {
  pinMode(SSpin, OUTPUT);
  SPI.begin(); 
}

void setup(void)
{
  //
  // Print preamble
  //

  Serial.begin(57600);
  printf_begin();
  printf("\n\rRF24/examples/GettingStarted/\n\r");
  printf("ROLE: %s\n\r",role_friendly_name[role]);
  printf("*** PRESS 'T' to begin transmitting to the other node\n\r");

  //
  // Setup and configure rf radio
  //

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
  radio.setAutoAck(true);
  // set the channel
  radio.setChannel(0x50);
  // set the power
  // RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
  radio.setPALevel(RF24_PA_MIN);
  //RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  radio.setDataRate(RF24_250KBPS);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  //radio.setPayloadSize(8);

  //
  // Open pipes to other nodes for communication
  //

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)

  if ( role == role_ping_out )
  {
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1,pipes[1]);
  }
  else
  {
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);
  }

  //
  // Start listening
  //

  radio.startListening();

  //
  // Dump the configuration of the rf unit for debugging
  //

  radio.printDetails();
}
void loop() {
    for(int i = 0; i<46; i++){
      if (i < 24) {
        sendInfo(trav[i]);
        sendInfo(0b10000000 | maze[trav[i]]);     
      }
      else {
        sendInfo(trav[23 - (i-23)]);
        sendInfo(0b10000000 | maze[trav[23 - (i-23)]]); 
      }
    }
}

void sendInfo(byte info){
  digitalWrite(SSpin,HIGH);
  SPI.transfer((byte) info);
  digitalWrite(SSpin,LOW);
  delay(300);
}

