#include <TimerOne.h>
#include <DIO2.h>
#include "TPLDCC2.h"
#include "TPLDCC.h"
#include "DIAG.h"

// This class handles the DCC transmissions for one track. (main or prog)
// Interrupts are marshalled via TPLDCC
// A track has a current transmit buffer, and a pending buffer.
// When the current buffer is exhausted, either the pending buffer (if there is one waiting) or an idle buffer. 
const byte bitMask[8]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
const int CURRENT_SAMPLE_MAX=300;

TPLDCC2::TPLDCC2(byte powerPinNo, byte directionPinNo, byte sensePinNo) {
   // establish appropriate pins 
   powerPin=Arduino_to_GPIO_pin(powerPinNo);
   directionPin=Arduino_to_GPIO_pin(directionPinNo);
   sensePin=sensePinNo;
   pinMode2f(powerPin,OUTPUT);
   pinMode2f(directionPin,OUTPUT);
   pinMode(sensePin,INPUT);
   packetPending=false;
   transmitPacket=TPLDCC::idlePacket;
   state=0;
   digitalWrite2f(powerPin,HIGH);
}
 void TPLDCC2:: mirror(TPLDCC2 otherTrack) {
      digitalWrite2f(directionPin,digitalRead2f(otherTrack.directionPin));
 }
 
void TPLDCC2::checkPowerOverload() {
  if (millis()<nextSampleDue) return;
  nextSampleDue=millis()+1;
  smoothedCurrent = (analogRead(sensePin) + smoothedCurrent*63) / 64;      // compute new exponentially-smoothed current

  // current overload and Signal is on
  if (smoothedCurrent > CURRENT_SAMPLE_MAX)
  {
    digitalWrite2f(powerPin, LOW);
    DIAG(F("\n*** POWER OVERLOAD ***\n"));                                     // print corresponding error message
  }
}

// process time-edge sensitive part of interrupt
// return true if second level required  
bool TPLDCC2::interrupt1() {

  switch (state) {
    case 0:  // start of bit transmission
      digitalWrite2f(directionPin, HIGH);
      state = 1;
      return true; // must call interrupt2

    case 1:  // 58Ms after case 0
      if (currentBit) {
        digitalWrite2f(directionPin, LOW);
        state = 0;
      }
      else state = 2;
      break;
    case 2:  digitalWrite2f(directionPin, LOW);
      state = 3;
      break;
    case 3:  state = 0;
    break;
  }
       return false;
 
}
void TPLDCC2::interrupt2() {
  currentBit = transmitPacket.data[transmitPacket.bits_sent / 8]  & bitMask[ transmitPacket.bits_sent % 8];
  transmitPacket.bits_sent++;
  if (transmitPacket.bits_sent >= transmitPacket.bits) {
      // end of transmission buffer... repeat or switch to next message
      if (transmitPacket.repeats > 0) {
        transmitPacket.repeats--;
        transmitPacket.bits_sent = 0;
      }
      else {
        transmitPacket= packetPending ? pendingPacket : TPLDCC::idlePacket;
        packetPending=false;
      }
      }
    }
  
  // Wait until there is no packet pending, then make this pending  
void TPLDCC2::schedulePacket(DCCPacket newpacket) {
    while(packetPending) delay(1);
    pendingPacket=newpacket;
    packetPending=true;
  }
  
