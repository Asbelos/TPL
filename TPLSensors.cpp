#include "TPLSensors.h"
#include "DIAG.h"
Adafruit_MCP23017 TPLSensors::mcp;
void TPLSensors::init(byte maxsensors)
{
  int mx=maxsensors;
  DIAG(F("\nTPLSensors(%d)\n"),maxsensors);
  Wire.begin();
  mcp.begin();      // use default address 0
  for (int pin=0;pin<16;pin++) {
    mcp.pinMode(pin, INPUT);
    mcp.pullUp(pin,HIGH);
  }
}


bool TPLSensors::readSensor(byte pin)
{
   return !mcp.digitalRead(pin);
}
