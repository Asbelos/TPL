#include "TPLTurnout.h"
#include "PWMServoDriver.h"
#define SERVOLEFT  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVORIGHT  195 // This is the 'maximum' pulse length count (out of 4096)

void TPLTurnout::begin(int maxTurnouts){
     PWMServoDriver::begin( maxTurnouts);
  }
     
// Override Turnout::activate
void TPLTurnout::activate(bool state)  {
     data.tStatus=state;                            
     PWMServoDriver::setServo(data.id, state?SERVORIGHT:SERVOLEFT);
 }
   
