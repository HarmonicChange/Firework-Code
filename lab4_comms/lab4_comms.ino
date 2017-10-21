// Servo Modules:
#include <Servo.h>
// RF comm modules:
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

// General Robot Setup
Servo leftWheel, rightWheel;
int startPin = 2;
int lineMidLeft, lineMidRight, lineRight, lineLeft;             //Line Sensor Values Variables
int lineMidLeftPin, lineMidRightPin, lineRightPin, lineLeftPin; // Analog pins with line sensors
int toleranceForward = 200; //
int blackDetect = 600;      // Threshold above which sensors are reading a black line
 
// RF Hardware Setup
RF24 radio(9,10); // Set up nRF24L01 radio on SPI bus plus pins 9 & 10
const uint64_t pipes[2] = { 0x0000000002ALL, 0x000000002BLL }; // Radio pipe addresses for the 2 nodes to communicate.
typedef enum { role_ping_out = 1, role_pong_back } role_e; // The various roles supported by this sketch
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"}; // The debug-friendly names of those roles
role_e role = role_pong_back; // The role of the current running sketch

// RF Transmission Data
unsigned char maze[5][4]; // Declare 5x4 maze array
byte robot_loc;
byte robot_dir;

// Setup specific for turn:
int turn = 0;               // Determines stage of 8-figure turn, values 0-7

void setup() {
  initializeStuff();
  initializeRFStuff();
  waitForStart();
}

void loop() {
  // Update line sensor values
  updateLineSensors();
  printSensors();
  transmitRobot();

  //Reset turn to 0 if it finishes 8 turns of figure eight
  if (turn == 8) turn = 0; 
  
  // Detect a crossroads
  if (lineLeft > blackDetect && lineRight > blackDetect) { 
    if (turn == 0 || turn == 4) keepStraight();
    else if (turn > 0 && turn < 4) rightTurn();
    else leftTurn();
    turn += 1;   
  }
  
  // Detects that it's on the black line
  else if ((abs(lineMidLeft - lineMidRight) < toleranceForward)) walkForward();
  
  // If not on the black line, readjust
  else if (lineMidLeft >= lineMidRight) leftDrift();      
  else if (lineMidLeft < lineMidRight)  rightDrift();

}


