#include <arduino.h>
#include "TPLThrottle.h"

volatile int TPLThrottle::counter;


const byte encoderPinClick = 17;//
const byte encoderPinA = 19;//outputA  
const byte encoderPinB = 18;//outoutB  

// plaudits due to https://github.com/buxtronix/arduino/tree/master/libraries/Rotary
// State situation flags 
// No complete step yet.
#define DIR_NONE 0x0
// Clockwise step.
#define DIR_CW 0x10
// Anti-clockwise step.
#define DIR_CCW 0x20
/*
 * The below state table has, for each state (row), the new state
 * to set based on the next encoder output. From left to right in,
 * the table, the encoder outputs are 00, 01, 10, 11, and the value
 * in that position is the new state to set.
 */

#define R_START 0x0

// Use the full-step state table (emits a code at 00 only)
#define R_CW_FINAL 0x1
#define R_CW_BEGIN 0x2
#define R_CW_NEXT 0x3
#define R_CCW_BEGIN 0x4
#define R_CCW_FINAL 0x5
#define R_CCW_NEXT 0x6

const unsigned char ttable[7][4] = {
  // R_START
  {R_START,    R_CW_BEGIN,  R_CCW_BEGIN, R_START},
  // R_CW_FINAL
  {R_CW_NEXT,  R_START,     R_CW_FINAL,  R_START | DIR_CW},
  // R_CW_BEGIN
  {R_CW_NEXT,  R_CW_BEGIN,  R_START,     R_START},
  // R_CW_NEXT
  {R_CW_NEXT,  R_CW_BEGIN,  R_CW_FINAL,  R_START},
  // R_CCW_BEGIN
  {R_CCW_NEXT, R_START,     R_CCW_BEGIN, R_START},
  // R_CCW_FINAL
  {R_CCW_NEXT, R_CCW_FINAL, R_START,     R_START | DIR_CCW},
  // R_CCW_NEXT
  {R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START},
};

unsigned char TPLThrottle::state=R_START;

void TPLThrottle::begin() {
  pinMode(encoderPinClick,INPUT_PULLUP);
  pinMode(encoderPinA,INPUT_PULLUP);
  pinMode(encoderPinB,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(18), isrA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(19), isrA, CHANGE);
}

void TPLThrottle::isrA() {
  unsigned char pinstate = (digitalRead(encoderPinB) << 1) | digitalRead(encoderPinA);
  state = ttable[state & 0xf][pinstate];
  unsigned char result = state & 0x30;
  if (result == DIR_CW) {
    counter++;
    if (counter>32) counter=32;
 } 
 else if (result == DIR_CCW) {
    counter--;
    if (counter<-32) counter=-32;
  }
}

void TPLThrottle::zero() {
  noInterrupts();
  counter=0;
  interrupts();
}
  
int TPLThrottle::count() {
  if (!digitalRead(encoderPinClick)) return QUIT_MANUAL;
  noInterrupts();
  int mycounter=counter;
  interrupts();
  return mycounter;
}
