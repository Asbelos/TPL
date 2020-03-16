#include "TPL.h"


ROUTES

 ROUTE(5) 
   PROGTRACK(1)
   READ_LOCO
   PROGTRACK(0)
   
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
      SPEED(10)        // Just Pretend we are a loco approaching the turnout
      TR(0)            // This will switch quickly
      DELAY(50)
      STOP             // pretend my loco is stopped
      TL(0)            // This will do a nice slow switch
      DELAY(50)
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
   

void setup(){
   Serial.begin(115200); // for diagnostics 
   tplBegin(9,     // Arduino pin connected to prog track relay
            40,    // Arduino pin connected to sensor zero.
            10,    // Number of contiguous sensor pins
            22,    // Arduino pin for signal zero
            8,     // Number of contiguous signals (2 pins each)
            16);   // Number of turnouts 
  }

  void loop() {
    tplLoop();
  }
