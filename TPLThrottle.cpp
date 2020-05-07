#include <DIO2.h>

#include <arduino.h>
#include "TPLThrottle.h"

volatile int TPLThrottle::counter;


const GPIO_pin_t encoderPinClick = DP17;//
const GPIO_pin_t encoderPinA = DP19;//outputA  
const GPIO_pin_t encoderPinB = DP18;//outoutB  
const GPIO_pin_t rowPins[]={DP30,DP31,DP32,DP33};
const GPIO_pin_t columnPins[]={DP34,DP35,DP36,DP37};
const char keyPadkeys[]="123A456B789C*0#D"; 

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
  // Rotary encoder
  pinMode2f(encoderPinClick,INPUT_PULLUP);
  pinMode2f(encoderPinA,INPUT_PULLUP);
  pinMode2f(encoderPinB,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(GPIO_to_Arduino_pin(encoderPinB)), isrA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(GPIO_to_Arduino_pin(encoderPinA)), isrA, CHANGE);

  // Matrix pad
  for (byte rc=0; rc<4; rc++) {
    pinMode2f(rowPins[rc],INPUT_PULLUP);
    pinMode2f(columnPins[rc],OUTPUT);
    digitalWrite2f(columnPins[rc],HIGH);
  }
  
}

bool TPLThrottle::quit() {
    return !digitalRead2f(encoderPinClick);
}

void TPLThrottle::isrA() {
  unsigned char pinstate = (digitalRead2f(encoderPinB) << 1) | digitalRead2f(encoderPinA);
  state = ttable[state & 0xf][pinstate];
  unsigned char result = state & 0x30;
  if (result == DIR_CW) {
    if (counter<32) counter++;
 } 
 else if (result == DIR_CCW) {
    if (counter>-32) counter--;
  }
}

void TPLThrottle::zero() {
  noInterrupts();
  counter=0;
  interrupts();
}
  
int TPLThrottle::count() {
  noInterrupts();
  int mycounter=counter;
  interrupts();
  return mycounter;
}

char TPLThrottle::getKey() {
   char result='\0';
  // just find one key
  for (byte c=0; c<4 && result=='\0'; c++) {
    digitalWrite2f(columnPins[c], LOW); // poll column
    for (byte r=0; r<4; r++) {
      if (digitalRead2f(rowPins[r])==LOW) result=keyPadkeys[4*r+c];
    }
    // Set pin to high impedance input. Effectively ends column pulse.
    digitalWrite2f(columnPins[c],HIGH);
  }
    return result;
}
