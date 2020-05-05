#include <arduino.h>
#define DIAG_ENABLED true
#include "DIAG.h"
#include "TPLTurnout.h"
#include "PWMServoDriver.h"
#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  195 // This is the 'maximum' pulse length count (out of 4096)
#define SERVO_STEPS 5 // steps through the slow move 
const short TURNOUT_DELAYER=50; // ms between steps 
const short MAX_TURNOUTS=16;
short currentPos[MAX_TURNOUTS];

void TPLTurnout::begin(){
     PWMServoDriver::begin(MAX_TURNOUTS);
     for (short id=0;id<MAX_TURNOUTS; id++) {
      PWMServoDriver::setServo(id,SERVOMIN);
      currentPos[id]=SERVOMIN;
     }
  }
     
    short TPLTurnout::slowSwitch(byte id, bool left, bool expedite) {
       if (id>MAX_TURNOUTS) return 0;  
      if (left) {
          if (currentPos[id]<=SERVOMIN) return 0;
          if (expedite)currentPos[id]=SERVOMIN;
          else currentPos[id]-=SERVO_STEPS;
      }
      else {
          if (currentPos[id]>=SERVOMAX) return 0;
          if (expedite) currentPos[id]=SERVOMAX;
          else currentPos[id]+=SERVO_STEPS;
      }
          PWMServoDriver::setServo(id,currentPos[id]);
          return expedite?0:millis()+TURNOUT_DELAYER;
  }
   
