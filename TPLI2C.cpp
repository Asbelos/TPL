#include <Arduino.h>
#include "DIAG.h"
// This file wraps all the Wire calls... making it a central point for replacing the Wire libraray easily
// It also provides some helper modules to replace commonly repeated blocks of code. 
#include "TPLI2C.h"
#include <Wire.h>

void TPLI2C::begin() {
  Wire.begin();
  DIAG(F("\nScanning I2C: "));
  for (byte address = 1; address < 127; ++address) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) DIAG(F("0x%S%x"), (address < 16)?F("0"):F(""),address);
    }
  DIAG(F("\n"));
}

void TPLI2C::write(uint8_t i2caddr,byte buffer[], byte size) {
  Wire.beginTransmission(i2caddr);
  for (int i=0;i<size;i++) Wire.write(buffer[i]);
  Wire.endTransmission();
}


void TPLI2C::writeRegister(uint8_t i2caddr,uint8_t hardwareRegister, uint8_t d) { 
  byte buffer[]={hardwareRegister,d};
  write(i2caddr,buffer,sizeof(buffer));
}


uint16_t TPLI2C::read2Registers(byte deviceAddr,byte reg)
{
  Wire.beginTransmission(deviceAddr);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(deviceAddr,(byte)2);
  //while (!Wire.available());
  uint16_t lsb = Wire.read();
  //while (!Wire.available());
  uint16_t msb = Wire.read();
  Wire.endTransmission();
  return ((msb << 8) | lsb);
}
