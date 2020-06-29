#include <Arduino.h>
#include "TPL.h"
#include "TPL2.h"
#include <Turnouts.h>
void TPL::begin(
                short signalZeroPin, // arduino pin connected to first signal
                short signals       // Number of signals (2 pins each)
                ) {
                  TPL2::begin(  signalZeroPin, signals);
                }

 
void TPL::loop() {
  TPL2::loop();
}

void TPL::I2CTURNOUT(byte id,  byte pin, int servoLeft, int servoRight) {
   Turnout::create(id,  pin, servoLeft, servoRight);
}
void TPL::DCCTURNOUT(byte id, int dccAddress, byte subAddress, bool activatedIsLeft) {
  Turnout::create(id, dccAddress, subAddress, activatedIsLeft);
}
