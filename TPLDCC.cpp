#include <TimerOne.h>
#include <DIO2.h>
#include "TPLDCC2.h"
#include "TPLDCC.h"
#include "DIAG.h"



TPLDCC2  TPLDCC::mainTrack(MAIN_POWER_PIN,MAIN_SIGNAL_PIN,MAIN_SENSE_PIN);
TPLDCC2  TPLDCC::progTrack(PROG_POWER_PIN,PROG_SIGNAL_PIN,PROG_SENSE_PIN);

byte TPLDCC::priorityReg=0;  // position of loop in loco speed refresh cycle
DCCPacket TPLDCC::locoPackets[MAX_LOCOS];
DCCPacket TPLDCC::idlePacket;

const byte idleMessage[3]={0xFF,0x00,0};

 void TPLDCC::begin() {
   formatPacket(idlePacket,(byte*)idleMessage,2,0,0);
   Timer1.initialize(58);
   Timer1.attachInterrupt(interruptHandler);
   Timer1.disablePwm(MAIN_SIGNAL_PIN);
   Timer1.disablePwm(PROG_SIGNAL_PIN);
}

 void TPLDCC::loop() {
  mainTrack.checkPowerOverload();
  progTrack.checkPowerOverload();
  
  // if the transmitter still has a pending packet, skip this loop.
  if (mainTrack.packetPending) return;

  // each time around the Arduino loop, we resend a loco speed packet reminder 
  for (; priorityReg < MAX_LOCOS; priorityReg++) {
    if (locoPackets[priorityReg].loco > 0) {
      mainTrack.schedulePacket(locoPackets[priorityReg]);
      priorityReg++;
      return;
    }
  }
  for (priorityReg = 0; priorityReg < MAX_LOCOS; priorityReg++) {
    if (locoPackets[priorityReg].loco > 0) {
      mainTrack.schedulePacket(locoPackets[priorityReg]);
      priorityReg++;
      return;
    }
  }
 }


 void TPLDCC::setSpeed(int loco, byte tSpeed, bool forward) {
  // create speed msg
  byte b[5];                      // save space for checksum byte
  byte nB = 0;

  if (loco > 127)
    b[nB++] = highByte(loco) | 0xC0;      // convert train number into a two-byte address

  b[nB++] = lowByte(loco);
  b[nB++] = 0x3F;                        // 128-step speed control byte
  b[nB++] = tSpeed + (tSpeed > 0) + forward * 128; // max speed is 126, but speed codes range from 2-127 (0=stop, 1=emergency stop)
  

  // determine speed reg for this loco
  int reg;
  for (reg = 0; reg < MAX_LOCOS; reg++) {
    int thisloco = locoPackets[reg].loco;
    if (thisloco == loco || thisloco == 0) break;
  }
  if (reg >= MAX_LOCOS) {
    DIAG(F("\nToo many locos\n"));
    return;
  }

  // format a packet from this message
  formatPacket(locoPackets[reg], b, nB, loco,0);
  priorityReg = reg;
}

void TPLDCC::formatPacket(DCCPacket newpacket, byte *b, byte nBytes, int loco , byte repeats ) {
  newpacket.loco = loco;
  newpacket.repeats = repeats;
  newpacket.bits_sent = 0;

  // Fill in the message checksum
  b[nBytes] = b[0];                      // copy first byte into what will become the checksum byte
  for (int i = 1; i < nBytes; i++)       // XOR remaining bytes into checksum byte
    b[nBytes] ^= b[i];
  nBytes++;                              // increment number of bytes in message to include checksum byte

  // Build the complete packet
  newpacket.data[0] = 0xFF;                      // first 8 bytes of 22-byte preamble
  newpacket.data[1] = 0xFF;                      // second 8 bytes of 22-byte preamble
  newpacket.data[2] = 0xFC + bitRead(b[0], 7);   // last 6 bytes of 22-byte preamble + data start bit + b[0], bit 7
  newpacket.data[3] = b[0] << 1;                 // b[0], bits 6-0 + data start bit
  newpacket.data[4] = b[1];                      // b[1], all bits
  newpacket.data[5] = b[2] >> 1;                 // b[2], bits 7-1
  newpacket.data[6] = b[2] << 7;                 // b[2], bit 0

  if (nBytes == 3) {
    newpacket.bits = 49;
  } else {
    newpacket.data[6] += b[3] >> 2;              // b[3], bits 7-2
    newpacket.data[7] = b[3] << 6;               // b[3], bit 1-0
    if (nBytes == 4) {
      newpacket.bits = 58;
    } else {
      newpacket.data[7] += b[4] >> 3;            // b[4], bits 7-3
      newpacket.data[8] = b[4] << 5;             // b[4], bits 2-0
      if (nBytes == 5) {
        newpacket.bits = 67;
      } else {
        newpacket.data[8] += b[5] >> 4;          // b[5], bits 7-4
        newpacket.data[9] = b[5] << 4;           // b[5], bits 3-0
        newpacket.bits = 76;
      } // >5 bytes
    } // >4 bytes
  } // >3 bytes
} // completePacket


// static //
void TPLDCC::interruptHandler() {
   // call the timer edge sensitive actions for progtrack and maintrack
   bool mainCall2=mainTrack.interrupt1();
   bool progCall2=progTrack.interrupt1();
   // call (if necessary) the procs to get the current bits
   // these must complete within 50microsecs of the interrupt
   // but they are only called ONCE PER BIT TRANSMITTED after the rising edge of the signal
   if (mainCall2) mainTrack.interrupt2();
   if (progCall2) progTrack.interrupt2();
}



void TPLDCC::schedulePacketMain(DCCPacket packet) {
     mainTrack.schedulePacket(packet);
  }
  
void TPLDCC::schedulePacketProg(DCCPacket packet) {
     progTrack.schedulePacket(packet);
  }
  
   
