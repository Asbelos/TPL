#include <arduino.h>
#include "TPLThrottle.h"

int TPLThrottle::counter;
byte TPLThrottle::aLastState;

void TPLThrottle::begin() {
  pinMode(5,INPUT);
  pinMode(6,INPUT);
  pinMode(7,INPUT);
  aLastState=digitalRead(6);
}
void TPLThrottle::zero() {
  counter=0;
}

int TPLThrottle::count() {
  return counter;
}

void TPLThrottle::loop() {
  byte aState=digitalRead(6);
  if (aState==aLastState) return;
  if (aState !=digitalRead(5)) counter--;
  else counter++;
  aLastState=aState;
  Serial.print("Counter:");
  Serial.println(counter);
}
