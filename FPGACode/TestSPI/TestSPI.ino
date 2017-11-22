/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Example for Getting Started with nRF24L01+ radios.
 *
 * This is an example of how to use the RF24 class.  Write this sketch to two
 * different nodes.  Put one of the nodes into 'transmit' mode by connecting
 * with the serial monitor and sending a 'T'.  The ping node sends the current
 * time to the pong node, which responds by sending the value back.  The ping
 * node can then see how long the whole cycle took.
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

int SPI_cs_pin = 3;
int SPI_data_pin = 4;
int SPI_clock_pin = 5;

// Robot Info:
byte robotLoc;
char trav[24] = {
  0,  1,  2,  3,
  7,  6,  5,  4,
  8, 12,  8,  4,
  5,  9, 13, 17,
 16, 17, 18, 19,
 15, 11, 10, 14, 
};
char maze[20] = {
  0b000000,  0b000000,  0b000000,  0b000000,
  0b000000,  0b000000,  0b000000,  0b000000,
  0b000000,  0b000000,  0b000000,  0b000000,
  0b000000,  0b000000,  0b000000,  0b000000,
  0b000000,  0b000000,  0b000000,  0b000000,

};

// Hardware configuration
RF24 radio(9,10);   // Set up nRF24L01 radio on SPI bus plus pins 9 & 10
const uint64_t pipes[2] = { 0x000000002ALL, 0x000000002BLL };   // Radio pipe addresses for the 2 nodes to communicate.


void setup(void)
{
  //
  // Print preamble
  //

  Serial.begin(57600);
  printf_begin();
  printf("ROLE: Receiver");

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
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);

  radio.startListening();
  radio.printDetails();


  // Setup SPI
  pinMode(SPI_cs_pin, OUTPUT);
  pinMode(SPI_data_pin, OUTPUT);
  pinMode(SPI_clock_pin, OUTPUT);
  digitalWrite(SPI_cs_pin, LOW);      //Initialize cs to be LOW
  digitalWrite(SPI_clock_pin, LOW);   //Initialize clock to be LOW
}

void loop(void)
{
  for (int i = 19; i >= 0; i++){
    robotLoc = trav[i];
    sendSPI(robotLoc);
    sendSPI(maze[robotLoc]);
  }
  
}

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
// vim:cin:ai:sts=2 sw=2 ft=cpp
