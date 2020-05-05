#include "TPL.h"


ROUTES
    SETLOCO(3)   
    FOLLOW(6) // turnout waver
    
 ROUTE(77)   
   MANUAL
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
   MANUAL
   
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
      MANUAL
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
   MANUAL
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
   

void setup(){
   Serial.begin(115200); // for diagnostics 
   TPL::begin(9,     // Arduino pin connected to prog track relay
            10,    // Number of contiguous sensor pins
            22,    // Arduino pin for signal zero
            8,     // Number of contiguous signals (2 pins each)
            16);   // Number of turnouts 
  }

  void loop() {
    TPL::loop();
    }
