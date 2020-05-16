#include <Arduino.h>
#include "TPL.h"
#include "TPL2.h"
TPL2 tpl2;
void TPL::begin(short sensors,       // number of sensors on I2C bus
                short signalZeroPin, // arduino pin connected to first signal
                short signals,       // Number of signals (2 pins each)
                short turnouts        // number of turnouts on I2C bus
                ) {
                  tpl2.begin( sensors, signalZeroPin, signals, turnouts);
                }

 
void TPL::loop() {
  tpl2.loop();
}
