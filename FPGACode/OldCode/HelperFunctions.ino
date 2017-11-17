
//------------------------------------------------------------------------------
// Radio Transmission Functions
//------------------------------------------------------------------------------
void initializeRFStuff(){
  // Print RF Preamble
  printf_begin();
  printf("\n\rRF24/examples/GettingStarted/\n\r");
  printf("ROLE: %s\n\r",role_friendly_name[role]);
  printf("*** PRESS 'T' to begin transmitting to the other node\n\r");

  // Setup radio transmission
  radio.begin();
  radio.setRetries(15,15); // optionally, increase the delay between retries & # of retries
  radio.setAutoAck(true);
  radio.setChannel(0x50); // Set Channel
  radio.setPALevel(RF24_PA_MAX); // Set Power, // RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
  radio.setDataRate(RF24_250KBPS); // Set Rate, //RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  //radio.setPayloadSize(8); // optionally, reduce the payload size. Seems to improve reliability
  // Open pipes to other nodes for communication
  if ( role == role_ping_out ) {
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1,pipes[1]); }
  else {
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]); }
  radio.startListening(); // Start Listening
  radio.printDetails(); // Dump the configuration of the rf unit for debugging
}

byte joinRobot(){
  return (robot_dir << 5) + (robot_loc);
}

void transmitRobot() {
  radio.stopListening();
  bool ok = radio.write( joinRobot(), sizeof(byte));
  printf("Now sending robot data...");         
  if (ok) printf("ok...");
  else printf("failed :( \n\r");

  // Data sent, start listening
  radio.startListening();
  // Wait here until we get a response, or timeout (250ms)
  unsigned long started_waiting_at = millis();
  bool timeout = false;
  while ( ! radio.available() && ! timeout )
    if (millis() - started_waiting_at > 200 )
      timeout = true;

  // Describe the results
  if ( timeout ) printf("Failed, response timed out.\n\r");
  else
  {
    // Grab the response, compare, and send to debugging spew
    unsigned long got_time;
    radio.read( &got_time, sizeof(unsigned long) );
  
    // Spew it
    printf("Got response %lu, round-trip delay: %lu\n\r",got_time,millis()-got_time);
  }
  

}


//------------------------------------------------------------------------------
// Other Functions
//------------------------------------------------------------------------------

void initializeStuff(){
  // Setup Serial and pin directions
  Serial.begin(9600);
  pinMode(startPin, INPUT);

  // Initialize system variables
  leftWheel.attach(5);
  rightWheel.attach(3);
  lineMidLeftPin = A1;
  lineMidRightPin = A2;
  lineRightPin = A3;
  lineLeftPin = A0;

  // RF transmission Data
  maze[5][4] = 
  {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,  
  };
  robot_loc = 0;
  robot_dir = 0;
}


