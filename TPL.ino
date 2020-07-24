#include "TPL.h"
#include <DCCEXParser.h>
#include <WifiInterface.h>
#include <freeMemory.h>
int minMemory;

// Create a serial command parser... This is OPTIONAL if you don't need to handle JMRI type commands
// from the Serial port.
DCCEXParser  serialParser;

void printmemory() {
  Serial.print(F("\nFREEMEM="));
  Serial.println(minMemory);
}

#include "myRoutes.h"


void setup(){
   DIAGSERIAL.begin(115200); // for diagnostics 
   Serial1.begin(115200);
   WifiInterface::setup(Serial1, F("BTHub5-M6PT"), F("49de8d4862"),F("DCCEX"),F("TPL"),3532); // (3532 is 0xDCC decimal... )

   #include "myLayout.h"
   
   TPL::begin(
            22,    // Obsolete... parameters Arduino pin for signal zero
            0);     // Number of contiguous signals (2 pins each)
   
     minMemory=freeMemory(); 
     printmemory();
  }

  void loop() {
    TPL::loop();
  //  handle any incoming commands on USB connection
  serialParser.loop(DIAGSERIAL);

  //  handle any incoming WiFi traffic
     WifiInterface::loop(Serial1);

    int thismemory=freeMemory();
    if (thismemory<minMemory) {
       minMemory=thismemory;
       printmemory();
    }
  }

 
