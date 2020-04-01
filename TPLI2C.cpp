#include <Arduino.h>
#include <Wire.h>

#include "TPLI2C.h"

void TPLI2C::writeRegister(uint8_t i2caddr,uint8_t hardwareRegister, uint8_t d) {
  Wire.beginTransmission(i2caddr);
  Wire.write(hardwareRegister);
  Wire.write(d);
  Wire.endTransmission();
}


byte TPLI2C::readRegister(byte deviceAddr,byte reg)
{
  Wire.beginTransmission(deviceAddr);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddr, (byte)1);
  return Wire.read();
}
