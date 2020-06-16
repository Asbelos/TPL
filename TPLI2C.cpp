#include <Arduino.h>

// This file wraps all the Wire calls... making it a central point for replacing the Wire libraray easily
// It also provides some helper modules to replace commonly repeated blocks of code. 
#include "TPLI2C.h"
#include <Wire.h>

void TPLI2C::begin() {
  Wire.begin();
}

void TPLI2C::beginTransmission(uint8_t i2caddr) {
  Wire.beginTransmission(i2caddr);
}
void TPLI2C::write(uint8_t value) {
  Wire.write(value);
}

void TPLI2C::endTransmission() {
  Wire.endTransmission();
}


void TPLI2C::writeRegister(uint8_t i2caddr,uint8_t hardwareRegister, uint8_t d) {
  beginTransmission(i2caddr);
  write(hardwareRegister);
  write(d);
  endTransmission();
}


byte TPLI2C::readRegister(byte deviceAddr,byte reg)
{
  beginTransmission(deviceAddr);
  write(reg);
  endTransmission();
  Wire.requestFrom(deviceAddr, (byte)1);
  return Wire.read();
}
