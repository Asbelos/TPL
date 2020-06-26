#include <Arduino.h>
#include <DIAG.h>
#include <Adafruit_MCP23017.h>
#include "TPLSensor.h"

Adafruit_MCP23017 * TPLSensor::mcp[4] = {NULL, NULL, NULL, NULL};
TPLSensor * TPLSensor::firstSensor = NULL;

TPLSensor::TPLSensor(byte _id, bool _isI2C, byte _pin) {
  if (pin > 64) return; // just a waste of memory
  id = _id;
  isI2C = _isI2C;
  pin = _pin;
  nextSensor = firstSensor;
  firstSensor = this;
  if (isI2C) {
    byte board = pin / 16;
    byte subPin = pin % 16;
    if (!mcp[board]) {
      mcp[board] = new  Adafruit_MCP23017();
      mcp[board]->begin(board);      // use default address 0
    }
    mcp[board]->pinMode(pin, INPUT);
    mcp[board]->pullUp(pin, HIGH);
  }
  else pinMode(pin, INPUT_PULLUP);
}

bool TPLSensor::read(byte id) {
  for (TPLSensor* ss = firstSensor; ss ; ss = ss->nextSensor)  if (id == ss->id) return ss->read();
  return false;
}

bool TPLSensor::read() {
  if (isI2C) return ! mcp[pin / 16]->digitalRead(pin % 16);
  return !digitalRead(pin);
}
