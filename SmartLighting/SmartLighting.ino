/*
 * This application consists of a node of a distributed smart lighting system.
 */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "printf.h"

#define RADIO_ID 0x66

#define SENSOR_PIN A0
#define LED_PIN 3

#define FULL_POWER 1023
#define HALF_POWER 300

#define SENSOR_THRESHOLD 500

#define TIME_THRESHOLD 2000

#define SEND_INTERVAL 100
//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10 

RF24 radio(9,10);

//
// Topology
//

// Radio pipe addresses for the nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xE8E8F0F0E1LL };

//
// Role management
//
// Set up role.  This sketch uses the same software for all the nodes
// in this system.  Doing so greatly simplifies testing.  
//

// The various roles supported by this sketch
typedef enum { role_speak = 1, role_listen, role_none } role_e;

// The role of the current running sketch
role_e role = role_none;
role_e newRole = role_none;

unsigned long timeLastMessage = 0;

void setup(void)
{
  //
  // Print preamble
  //

  Serial.begin(57600);
  printf_begin();
  printf("\n\rSmart Lighing application\n\r");

  pinMode(SENSOR_PIN,INPUT);
  pinMode(LED_PIN,OUTPUT);

  //
  // Setup and configure rf radio
  //

  // optionally, increase the delay between retries & # of retries
//  radio.setPayloadSize(sizeof(uint8_t));

  radio.begin();
}

void loop(void)
{

 if(analogRead(SENSOR_PIN) > SENSOR_THRESHOLD){
  newRole = role_speak;
 } else {
  newRole = role_listen;
 }
 // If new role, change
 if(newRole != role){
  printf("Changed to role %d",newRole);
  role = newRole;
  if(role == role_listen){
   //radio.openWritingPipe(pipes[0]);
   timeLastMessage = millis();
   radio.openReadingPipe(1,pipes[1]);
   radio.startListening();
   radio.printDetails();
  } else {
   analogWrite(LED_PIN,FULL_POWER);
   radio.openWritingPipe(pipes[1]);
   //radio.openReadingPipe(1,pipes[0]);
   radio.stopListening();
   radio.printDetails();
  }
 }
  //
  // Ping out role.  Repeatedly send the current time
  //

  if (role == role_speak)
  {
    radio.startListening();
    radio.stopListening();
    printf("Radio %u is on!",RADIO_ID);
    uint8_t radio_id = RADIO_ID;
    bool ok = radio.write( &radio_id, sizeof(uint8_t) );
    
    if (ok)
      printf("ok...\n\r");
    else
      printf("failed.\n\r");

    delay(SEND_INTERVAL);
  }

  //
  // Pong back role.  Receive each packet, dump it out, and send it back
  //

  if ( role == role_listen )
  {
    // if there is data ready
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      bool done = false;
      while (!done)
      {
        uint8_t signal_id = 0;
        // Fetch the payload, and see if this was the last one.
        done = radio.read( &signal_id, sizeof(uint8_t) );

        // Spew it
        printf("Got payload %u...",signal_id);

	// Delay just a little bit to let the other unit
	// make the transition to receiver
	delay(20);
      }
      analogWrite(LED_PIN,HALF_POWER);
      timeLastMessage = millis();
    }
    if(millis() - timeLastMessage > SEND_INTERVAL * 2){
      analogWrite(LED_PIN,LOW);
    }
  }
}
// vim:cin:ai:sts=2 sw=2 ft=cpp
