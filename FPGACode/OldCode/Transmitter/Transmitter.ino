/*s
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


// Hardware configuration for Transmitter
RF24 radio(9,10);  // Set up nRF24L01 radio on SPI bus plus pins 9 & 10
const uint64_t pipes[2] = { 0x000000002ALL, 0x000000002BLL };  // Radio pipe addresses for the 2 nodes to communicate.

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


void setup(void)
{
  //
  // Print preamble
  //

  Serial.begin(57600);
  printf_begin();
  printf("ROLE: Transmitter");

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
  radio.setPALevel(RF24_PA_HIGH);
  //RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);

  radio.startListening();
  radio.printDetails();
}

void loop(void)
{
  //
  // Ping out role.  Repeatedly send the current time
  //
    int i = 0;
    bool sendFailed = false;
    while (i < 46){
    //for(int i = 0; i<46; i++){
      if (i < 24) {
        bool send1 = sendRF(trav[i]);
        bool send2 = sendRF(0b10000000 | maze[trav[i]]);     
        sendFailed = send1 | send2;
      }
      else {
        bool send1 = sendRF(trav[23 - (i-23)]);
        bool send2 = sendRF(0b10000000 | maze[trav[23 - (i-23)]]); 
        sendFailed = send1 | send2;
      }
      if (!sendFailed) i = i + 1; 
    }
  

}


bool sendRF(byte sendData){
    // First, stop listening so we can talk.
    radio.stopListening();

    // Take the time, and send it.  This will block until complete
    byte time = sendData;
    printf("Now sending %x...",time);
    bool ok = radio.write( &time, sizeof(byte) );

    if (ok)
      printf("ok...");
    else
      printf("failed.\n\r");

    // Now, continue listening
    radio.startListening();

    // Wait here until we get a response, or timeout (250ms)
    unsigned long started_waiting_at = millis();
    bool timeout = false;
    while ( ! radio.available() && ! timeout )
      if (millis() - started_waiting_at > 200 )
        timeout = true;

    // Describe the results
    if ( timeout )
    {
      printf("Failed, response timed out.\n\r");
    }
    else
    {
      // Grab the response, compare, and send to debugging spew
      byte got_resp;
      radio.read( &got_resp, sizeof(byte) );

      // Spew it
      printf("Got response %x\n\r",got_resp);
    }

    // Try again 1s later
    delay(1000);
    return timeout;
}



// vim:cin:ai:sts=2 sw=2 ft=cpp
