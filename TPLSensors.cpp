#include <arduino.h>
#include "TPLSensors.h"
#include "TPLI2C.h"
#define DIAG_ENABLED true
#include "DIAG.h"

const byte BASE_SENSOR_DEVICE_ADDRESS=0x20;
const byte MCP23017_IOCON=0x0a;
const byte MCP23017_GPPUA=0x0C;
const byte MCP23017_GPPUB=0x0D;
const byte MCP23017_IODIRA=0x00;
const byte MCP23017_IODIRB=0x01;
const byte MCP23017_GPIOA=0x12;


void TPLSensors::init(byte maxsensors)
{ int mx=maxsensors;
  DIAG(F("TPLSensors(%d)\n"),maxsensors);
  for (byte deviceAddr=BASE_SENSOR_DEVICE_ADDRESS; mx>0; deviceAddr++) {
    DIAG(F("\nInit Device %d, mx=%d"),deviceAddr,mx); 
    TPLI2C::writeRegister(deviceAddr,MCP23017_IOCON, 0b00100000);
    //enable all pull up resistors (will be effective for input pins only)
    TPLI2C::writeRegister(deviceAddr,MCP23017_GPPUA, 0xFF);
    TPLI2C::writeRegister(deviceAddr,MCP23017_GPPUB, 0xFF);
    TPLI2C::writeRegister(deviceAddr,MCP23017_IODIRA, 0xFF);
    TPLI2C::writeRegister(deviceAddr,MCP23017_IODIRB, 0xFF);
    mx-=16;
  }

}

bool TPLSensors::readSensor(byte pin)
{
  byte deviceAddr=BASE_SENSOR_DEVICE_ADDRESS + (pin/16);
  pin%=16;
  //DIAG(F("\nRead Sensor devideAddr=%d, pin=%d"),deviceAddr,pin);
  byte value=TPLI2C::readRegister(deviceAddr,MCP23017_GPIOA + (pin/8));
  bool result=!( value & _BV(pin%8));
  //if (value!=0xff) DIAG(F("Read Sensor %d value=%x %d %d %d\n"),pin,value,_BV(pin%8), value & _BV(pin%8),result);
  return result;
}
