#include <DCCpp.h>
#define DIAG_ENABLED true
#include "DIAG.h"
#include "TPLTurnout.h"
#include <Adafruit_PWMServoDriver.h>
#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  200 // This is the 'maximum' pulse length count (out of 4096)
#define SERVO_STEPS 1 // steps through the slow move 
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates
Adafruit_PWMServoDriver pwmDriver ;
    void TPLTurnout::SetTurnouts(short _turnoutBoard0, short _turnouts){
    pwmDriver.begin(); 
    pwmDriver.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

    int subAddress=0;
    for (int id = 0; id < _turnouts ; id++) {
      TPLTurnout::create(id,_turnoutBoard0,subAddress);
      subAddress++;
      if (subAddress==16) {
        _turnoutBoard0++; // not yet handled correctly with multiple board drivers
         subAddress=0;
      }
    }
  }

    TPLTurnout* TPLTurnout::create(int id, int add, int subAdd) {
    TPLTurnout *tt = new TPLTurnout();
    tt->begin(id, add, subAdd);
    DIAG("\n New Turnout %d %d %d",tt->data.id, tt->data.address, tt->data.subAddress);
    tt->activate(0);
     return(tt);
  }


  void TPLTurnout::activate(int s) {
     DIAG(" Turnout %d %d %d",data.id, data.subAddress,s);
     currentPos=s==0?SERVOMIN:SERVOMAX;
     pwmDriver.setPWM((int)data.subAddress, 0,currentPos );
     };
     
    bool TPLTurnout::slowSwitch(short num, bool left) {
      TPLTurnout* t=(TPLTurnout*)(Turnout::get(num));
      if (t==NULL) return true;
      delay(15); // testing - replace with polled loop
      DIAG(" Turnout %d %d %d",t->data.id, t->data.subAddress,left);
      if (left) {
          if (t->currentPos<=SERVOMIN) return true;
          t->currentPos-=SERVO_STEPS;
      }
      else {
          if (t->currentPos>=SERVOMAX) return true;
          t->currentPos+=SERVO_STEPS;
      }
          pwmDriver.setPWM((int)(t->data.subAddress),0,t->currentPos);
          return false;
  }
   
