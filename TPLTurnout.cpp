#include <DCCpp.h>
#define DIAG_ENABLED true
#include "DIAG.h"
#include "TPLTurnout.h"
#include "PWMServoDriver.h"
#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  200 // This is the 'maximum' pulse length count (out of 4096)
#define SERVO_STEPS 2 // steps through the slow move 
    
void TPLTurnout::SetTurnouts( short _turnouts){
     PWMServoDriver::begin(_turnouts);
    for (int id = 0; id < _turnouts ; id++) {
      TPLTurnout::create(id);
    }
  }

 TPLTurnout* TPLTurnout::create(int id) {
    TPLTurnout *tt = new TPLTurnout();
    tt->begin(id, id,id);
    tt->activate(0);
    return(tt);
  }


  void TPLTurnout::activate(int s) {
     DIAG(F(" Turnout %d %d %d"),data.id, data.subAddress,s);
     currentPos=s==0?SERVOMIN:SERVOMAX;
     PWMServoDriver::setServo(data.id,currentPos );
     };
     
    bool TPLTurnout::slowSwitch(short num, bool left, bool expedite) {
      TPLTurnout* t=(TPLTurnout*)(Turnout::get(num));
      if (t==NULL) return true;
      DIAG(F("\nTurnout %d %d"),t->data.id,left);
      if (left) {
          if (t->currentPos<=SERVOMIN) return true;
          if (expedite) t->currentPos=SERVOMIN;
          else t->currentPos-=SERVO_STEPS;
      }
      else {
          if (t->currentPos>=SERVOMAX) return true;
          if (expedite) t->currentPos=SERVOMAX;
          else t->currentPos+=SERVO_STEPS;
      }
          PWMServoDriver::setServo(t->data.id,t->currentPos);
          return expedite;
  }
   
