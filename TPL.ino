#include "TPL.h"
#include <DCCEXParser.h>
#include <WifiInterface.h>

ROUTES

    // testing sensor code 
    AT(0)
    AT(1)
    AFTER(2)
    FOLLOW(0)

/// 


    SETLOCO(99)   
 ROUTE(103)
// MANUAL
 FOLLOW(103) 
    
 ROUTE(77)   
//   MANUAL
   DELAY(50)
   FOLLOW(77 )
   //SCHEDULE(55)
 ROUTE(1)
   AT(0)
   TL(0)
   AT(1)
   TR(0)
   FOLLOW(1)
   ENDROUTE
   
 ROUTE(5) 
   DELAY(100)
   PROGTRACK(1)
   DELAY(50)
   READ_LOCO
   PROGTRACK(0)
//   MANUAL
   
  ROUTE(55)
   FON(0)
   FON(1)
   FON(2)
   FON(3)
   FON(4)
   FON(5)
   FON(6)
   FON(7)
   
   DELAY(50)
   FOFF(0)
   FOFF(1)
   FOFF(2)
   FOFF(3)
   FOFF(4)
   FOFF(5)
   FOFF(6)
   FOFF(7)
   DELAY(50)
   FOLLOW(55)
   

   ROUTE(6)  // Just exercises Turnout 0
//      MANUAL
      SPEED(1) 
      TR(0)            // This will switch quickly
      DELAY(5)
       TR(1)            // This will switch quickly
      DELAY(5)
       TR(2)            // This will switch quickly
      DELAY(5)
       TR(3)            // This will switch quickly
      DELAY(5)
       TR(4)            // This will switch quickly
      DELAY(5)
       TR(8)            // This will switch quickly
      DELAY(5)
       TR(9)            // This will switch quickly
      DELAY(5)
       TR(10)            // This will switch quickly
      DELAY(5)
       TR(11)            // This will switch quickly
//   MANUAL
       SPEED(1)
       
      DELAY(5)
       TL(0)            // This will switch quickly
      DELAY(5)
       TL(1)            // This will switch quickly
      DELAY(5)
       TL(2)            // This will switch quickly
      DELAY(5)
       TL(3)            // This will switch quickly
      DELAY(5)
       TL(4)            // This will switch quickly
      DELAY(5)
       TL(8)            // This will switch quickly
      DELAY(5)
       TL(9)            // This will switch quickly
      DELAY(5)
       TL(10)            // This will switch quickly
      DELAY(5)
       TL(11)            // This will switch quickly  
      FOLLOW(6)        // Go back and do it again 

   ROUTE(90)   // Launch controller
      AFTER(0)    // Sensor 0 is a button on control panel
      PROGTRACK(1)  // program Track is in program mode
      READ_LOCO  // Read the loco address etc on the progtrack
      PROGTRACK(0) // Connect progtrack to main track
                   // Drop through to ROUTE(91)
                   
   ROUTE(91)       // moves off prog track and reverses if going wrong way
      FWD(10)    // Move off slowly
      IF(1)      // sensor 1 is at the buffer end of progtrack!
        INVERT_DIRECTION    // Reverse all directions for this loco
        AT(2)      // wait until sensor 2  on main track just after end of the progtrack
        FOLLOW(92) // see below
        ENDIF
      IF(2)
        FOLLOW(92)
        ENDIF
      FOLLOW(91)   // keep checking

    ROUTE(92) 
      ENDROUTE        

   ENDROUTES


#include <freeMemory.h>
int minMemory;

// Create a serial command parser... This is OPTIONAL if you don't need to handle JMRI type commands
// from the Serial port.
DCCEXParser  serialParser;


void printmemory() {
  Serial.print(F("\nFREEMEM="));
  Serial.println(minMemory);
}


void setup(){
   DIAGSERIAL.begin(115200); // for diagnostics 
    Serial1.begin(115200);
    WifiInterface::setup(Serial1, F("BTHub5-M6PT"), F("49de8d4862"),F("DCCEX"),F("TPL"),3532); // (3532 is 0xDCC decimal... )

   TPL::begin(
            22,    // Arduino pin for signal zero
            0);     // Number of contiguous signals (2 pins each)

   // Describe the turnouts (use  a loop if it helps) 
   // I2C turnouts  idNum = id used in TL() and TR() and JMRI (if used)
   //               pin   = pin position relative to first PWM servo board (second board starts at 16 etc)
   //               leftAngle  = servo position for turn left    
   //               rightAngle = servo position for turn right
   //DCC turnouts can be added if you have them.    
   TPL::I2CTURNOUT(0, 0, 150, 195);       
   TPL::I2CTURNOUT(1, 1, 150, 195);       
   TPL::I2CTURNOUT(2, 2, 150, 195);       
   TPL::I2CTURNOUT(3, 3, 150, 195);       
   TPL::I2CTURNOUT(4, 4, 150, 195);       
   TPL::I2CTURNOUT(5, 5, 150, 195);       
   TPL::I2CTURNOUT(6, 8, 150, 195);       
   TPL::I2CTURNOUT(7, 9, 150, 195);       
   TPL::I2CTURNOUT(8,11, 150, 195);       
   
     minMemory=freeMemory(); 
     printmemory();
  }

  void loop() {
    TPL::loop();
  //  handle any incoming commands on USB connection
  serialParser.loop(DIAGSERIAL);

  //  handle any incoming WiFi traffic
  WifiInterface::loop(Serial1);

    int thismemory=freeMemory();
    if (thismemory<minMemory) {
       minMemory=thismemory;
       printmemory();
    }
  }

 
