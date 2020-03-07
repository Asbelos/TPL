#include "TPL.h"
#include "autotask.h"
#include "TPLTurnout.h"
#define DIAG_ENABLED true
#include "DIAG.h"
#include <DCCpp.h>

#ifndef USE_TEXTCOMMAND
#error To be able to compile this sample,the line #define USE_TEXTCOMMAND must be uncommented in DCCpp.h
#endif

  // a route drives an engine over some part of the track
 ROUTE(Route1) // 1-8-3-7-2
  AT(0)  
  DELAY(50,200)  RESERVE(8)  RESERVE(5)   TL(1)  GREEN(1)  RESET(9)  RESET(5)  FWD(25)
  AT(9)  RED(1) 
  AT(5)  FREE(1)  RESERVE(9)  TL(3)  TL(4)  GREEN(5)  RESET(8)  FWD(20)  
  AT(8)  STOP  RED(5)  FREE(5)  FREE(9)  
  DELAY(100,200)  RESERVE(3)  RESERVE(6)  RESERVE(9)  TL(4)  TR(3)  TL(2)  GREEN(8)  RESET(6)  RESET(3)  REV(35)
  AT(6)  FREE(3)  RED(8)  FREE(8)  FREE(9)  
  AT(3)  STOP  FREE(6)
  DELAY(100,200)  RESERVE(7)  RESERVE(6)  TL(2)  GREEN(3)  RESET(10)  RESET(6)  FWD(25)
  AT(10)  RED(3)  
  AT(6)  RESERVE(9)  TR(3)  TR(4)  GREEN(6)  FWD(25)
  AT(7)  STOP  RED(6)  FREE(6)  FREE(9)  
  DELAY(100,200)  RESERVE(2)  RESERVE(5)  TL(1)  RESERVE(9)  TR(4)  TL(3)  GREEN(7)  RESET(5)  RESET(2)  REV(5)
  AT(5)  RED(7)  FREE(7)  FREE(9)  TR(1)  
  AT(2)  STOP  FREE(5)
  FOLLOW(2)
 };
  
  // a route drives an engine over some part of the track
 ROUTE(Route2) // 2-8-4-7-1
  AT(0)
  DELAY(50,200)
  RESERVE(8)
  RESERVE(5) 
  TR(1)
  GREEN(2)
  RESET(9)
  RESET(5)
  FWD(5)
  AT(9)
  RED(2)
  AT(5)
  STOP
  FREE(2)
  RESERVE(9)
  TL(3)
  TL(4)
  GREEN(5)
  RESET(8)
  FWD(5)
  AT(8)
  STOP
  RED(5)
  FREE(5)
  FREE(9)
  DELAY(100,200)
  RESERVE(4)
  RESERVE(6)
  RESERVE(9)
  TL(4)
  TR(3)
  TR(2)
  GREEN(8)
  RESET(6)
  RESET(4)
  REV(5)
  AT(6)
  RED(8)
  FREE(8)
  FREE(9)
  AT(4)
  STOP
  FREE(6)
  DELAY(100,200)
  RESERVE(7)
  RESERVE(6)
  TR(2)
  GREEN(4)
  RESET(10)
  RESET(6)
  FWD(5)
  AT(10)
  FREE(4)
  RED(4)
  AT(6)
  RESERVE(9)
  TR(3)
  TR(4)
  GREEN(6)
  RESET(7)
  FWD(5)
  AT(7)
  STOP
  RED(6)
  FREE(6)
  FREE(9)
  DELAY(100,200)
  RESERVE(1)
  RESERVE(5)
  TR(1)
  RESERVE(9)
  TR(4)
  TL(3)
  GREEN(7)
  RESET(5)
  RESET(1)
  REV(5)
  AT(5)
  TL(1)
  RED(7)
  FREE(7)
  FREE(9)
  AT(1)
  STOP
  FREE(5)
  FOLLOW(1)
 };

  ROUTE(Setup)
  RESET(0)
  RED(1)
  RED(2)
  RED(3)
  RED(4)
  RED(5)
  RED(6)
  RED(7)
  RED(8)
  RESERVE(1)
  RESERVE(2)
  SET(0)
  ENDPROG
 };

 ROUTE(Tester) 
   DELAY(50,50)
   PROGTRACK(0)
   TR(0)
   DELAY(50,50)
   TL(0)
   PROGTRACK(1)
   
   AGAIN
   };
   
 // This array allows routes to be followed by number 
const short *  Routes[]={Setup,Route1,Route2, Tester};


void setup(){
   Serial.begin(115200);
   DIAG("\n*** SETUP ***\n");
   DCCpp::begin();
   DCCpp::beginMainMotorShield();
   DCCpp::beginProgMotorShield();
   tplSensorZeroPin(40,10);
   tplSignalsZeroPin(22,8);
   tplProgTrackPin(9);
   TPLTurnout::SetTurnouts(0x40,16);
   tplAddRoutes(Routes);
   // tplAddTask(Setup);
   //tplAddJourney(Route1,0,3,16);
   // tplAddJourney(Route2,1,4,127);
   tplAddJourney(Tester,2,99,127);
  }

  void loop() {
    DCCpp::loop();
    tplLoop();
  }
