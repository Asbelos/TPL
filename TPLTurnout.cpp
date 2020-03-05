#include <DCCpp.h>
#include "TPLTurnout.h"
#define DIAG_ENABLED true
#include "DIAG.h"
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates
static Adafruit_PWMServoDriver pwmDriver ;

  void tplTurnout::setup(short _turnoutBoard0, short _turnouts){
    pwmDriver=Adafruit_PWMServoDriver();
    pwmDriver.begin();
  // In theory the internal oscillator is 25MHz but it really isn't
  // that precise. You can 'calibrate' by tweaking this number till
  // you get the frequency you're expecting!
  pwmDriver.setOscillatorFrequency(27000000);  // The int.osc. is closer to 27MHz  
  pwmDriver.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

    int subAddress=0;
    for (int id = 0; id < _turnouts ; id++) {
      tplTurnout::create(id,_turnoutBoard0,subAddress);
      subAddress++;
      if (subAddress==16) {
        _turnoutBoard0++; // not yet handled correctly with multiple board drivers
         subAddress=0;
      }
    }
  }

  tplTurnout* tplTurnout::create(int id, int add, int subAdd) {
    tplTurnout *tt = new tplTurnout();
    tt->begin(id, add, subAdd);
    DIAG("\n New Turnout %d %d %d",tt->data.id, tt->data.address, tt->data.subAddress);
    return(tt);
  }

  void tplTurnout::activate(int s) {
     DIAG(" Turnout %d %d %d",data.id, data.subAddress,s);
     pwmDriver.setPWM((int)data.subAddress, 0, s==0?SERVOMIN:SERVOMAX);
     };
     
  void tplTurnout::activate(short num, bool left) {
      Turnout* t=Turnout::get(num);
      if (t!=NULL) t->activate(left);
      }
