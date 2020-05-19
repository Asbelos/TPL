#include <Arduino.h>
#include <DIO2.h>
#include <TimerThree.h>
#include "TPLDCC1.h"
#include "DIAG.h"

TPLDCC1  TPLDCC1::mainTrack(MAIN_POWER_PIN,MAIN_SIGNAL_PIN,MAIN_SENSE_PIN,true);
TPLDCC1  TPLDCC1::progTrack(PROG_POWER_PIN,PROG_SIGNAL_PIN,PROG_SENSE_PIN,false);
DCCPacket TPLDCC1::idlePacket;  // filled by TPLDCC at begin



void TPLDCC1::begin() {
   Timer3.initialize(58);
   Timer3.disablePwm(MAIN_SIGNAL_PIN);
   Timer3.disablePwm(PROG_SIGNAL_PIN);
   Timer3.attachInterrupt(interruptHandler);
   mainTrack.beginTrack();
   progTrack.beginTrack();   
}
 
 void TPLDCC1::loop() {
   mainTrack.checkPowerOverload();
   progTrack.checkPowerOverload();
 }
 

// static //
void TPLDCC1::interruptHandler() {
   // call the timer edge sensitive actions for progtrack and maintrack
   bool mainCall2=mainTrack.interrupt1();
   bool progCall2=progTrack.interrupt1();
   
   // call (if necessary) the procs to get the current bits
   // these must complete within 50microsecs of the interrupt
   // but they are only called ONCE PER BIT TRANSMITTED 
   // after the rising edge of the signal
   if (mainCall2) mainTrack.interrupt2();
   if (progCall2) progTrack.interrupt2();
}


// An instance of this class handles the DCC transmissions for one track. (main or prog)
// Interrupts are marshalled via the statics.
// A track has a current transmit buffer, and a pending buffer.
// When the current buffer is exhausted, either the pending buffer (if there is one waiting) or an idle buffer. 
const byte bitMask[8]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};


TPLDCC1::TPLDCC1(byte powerPinNo, byte directionPinNo, byte sensePinNo, bool isMain) {
   // establish appropriate pins 
   powerPin=Arduino_to_GPIO_pin(powerPinNo);
   directionPin=Arduino_to_GPIO_pin(directionPinNo);
   sensePin=sensePinNo;
   isMainTrack=isMain;
   packetPending=false;
   transmitPacket=idlePacket;
   state=0;
   bits_sent=0;
   nextSampleDue=0;
}
 void TPLDCC1::beginTrack() {
   pinMode2f(powerPin,OUTPUT);
   pinMode2f(directionPin,OUTPUT);
   pinMode(sensePin,INPUT);
   setPowerMode(POWERMODE::ON); 
   DIAG(F("\nTrack started sensePin=%d\n"),sensePin); 
 }

 POWERMODE TPLDCC1::getPowerMode() {
  return powerMode;
 }

 void TPLDCC1::setPowerMode(POWERMODE mode) {
  powerMode=mode;
  digitalWrite2f(powerPin, mode==POWERMODE::ON ? HIGH:LOW);
 }
 
 
void TPLDCC1::checkPowerOverload() {
  if (millis()<nextSampleDue) return;
  int current;
  int delay;
  
  switch (powerMode) {
    case POWERMODE::OFF: 
      delay=POWER_SAMPLE_OFF_WAIT;
      break;
   case POWERMODE::ON: 
      // Check current  
      current=analogRead(sensePin);
      if (current < POWER_SAMPLE_MAX)  delay=POWER_SAMPLE_ON_WAIT;
      else {
        setPowerMode(POWERMODE::OVERLOAD);
        DIAG(F("\n*** %s TRACK POWER OVERLOAD pin=%d current=%d max=%d ***\n"),isMainTrack?"MAIN":"PROG",sensePin,current,POWER_SAMPLE_MAX);
        delay=POWER_SAMPLE_OVERLOAD_WAIT;
      }
      break;
   case POWERMODE::OVERLOAD:
      // Try setting it back on after the OVERLOAD_WAIT
      setPowerMode(POWERMODE::ON);
      delay=POWER_SAMPLE_ON_WAIT;
      break;
    default:
      delay=999;  // cant get here..meaningless statement to avoid compiler warning.  
  }
  nextSampleDue=millis()+delay;
}





// process time-edge sensitive part of interrupt
// return true if second level required  
bool TPLDCC1::interrupt1() {
  // NOTE: this must consume transmission buffers even if the power is off 
  // otherwise can cause hangs in main loop waiting for the pendingBuffer. 
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
void TPLDCC1::interrupt2() {
  currentBit = transmitPacket.data[bits_sent / 8]  & bitMask[ bits_sent % 8 ];
  bits_sent++;
  if (bits_sent >= transmitPacket.bits) {
      bits_sent = 0;
      // end of transmission buffer... repeat or switch to next message
      if (transmitRepeats > 0) {
        transmitRepeats--;
      }
      else if (packetPending) {
        transmitPacket= pendingPacket;
        transmitRepeats= pendingRepeats;
        packetPending=false;
      }
      else {
        transmitPacket=idlePacket;
        transmitRepeats=0;
      }
    }
}

  // Wait until there is no packet pending, then make this pending  
void TPLDCC1::schedulePacket(DCCPacket& newpacket, byte repeats) {
    while(packetPending) delay(1);
    pendingPacket=newpacket;
    pendingRepeats=repeats;
    packetPending=true;
  }
 
// ACK stuff is untested!!!!
 bool  TPLDCC1::startAckProcess() {
  if (sensePin==0) return false; 
  int base=0;
  for (int j = 0; j < ACK_BASE_COUNT; j++)
  {
    base+= (int)analogRead(sensePin);
  }
  ackBaseCurrent=base / ACK_BASE_COUNT;
  return true; 
}

bool TPLDCC1::getAck()
{
  int threshold=ackBaseCurrent+ACK_SAMPLE_THRESHOLD;

  for (int j = 0; j < ACK_SAMPLE_COUNT; j++)
  {
    if (analogRead(sensePin) > threshold) return true;
  }
  return false;
}
