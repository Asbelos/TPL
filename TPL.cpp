
#include "TPL.h"
#include "TPL2.h"
TPL2 tpl2;
void TPL::begin(short progTrackPin,  // arduino pin connected to progtrack relay
                                     // e.g pin 9 as long as motor shield Brake links cut.
                                     // A relay attached to this pin will switch the programming track
                                     // to become part of the main track.  
                 short sensors,       // number of sensors on I2C bus
                short signalZeroPin, // arduino pin connected to first signal
                short signals,       // Number of signals (2 pins each)
                short turnouts        // number of turnouts on I2C bus
                ) {
                  tpl2.begin(progTrackPin, sensors, signalZeroPin, signals, turnouts);
                }

 
void TPL::loop() {
  tpl2.loop();
}
