#include <TimerThree.h>
#include <DIO2.h>
#include "TPLDCC2.h"
#include "TPLDCC1.h"
#include "TPLDCC.h"
#include "DIAG.h"





const byte idleMessage[3]={0xFF,0x00,0};

byte TPLDCC::priorityReg=0;  // position of loop in loco speed refresh cycle
DCCPacket TPLDCC::locoPackets[MAX_LOCOS];
DCCPacket TPLDCC::idlePacket;
 
void TPLDCC::begin() {
  formatPacket(idlePacket,(byte*)idleMessage,2,0,0);
  TPLDCC1::begin();
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





   
void TPLDCC::setFunction( int cab, int fByte, int eByte) 
{
 byte b[5];                      // save space for checksum byte
  byte nB = 0;

  if (cab>127)
    b[nB++] = highByte(cab) | 0xC0;      // convert train number into a two-byte address

  b[nB++] = lowByte(cab);

  if (eByte < 0) {                      // this is a request for functions FL,F1-F12  
    b[nB++] = (fByte | 0x80) & 0xBF;     // for safety this guarantees that first nibble of function byte will always be of binary form 10XX which should always be the case for FL,F1-F12  
  }
  else {                             // this is a request for functions F13-F28
    b[nB++] = (fByte | 0xDE) & 0xDF;     // for safety this guarantees that first byte will either be 0xDE (for F13-F20) or 0xDF (for F21-F28)
    b[nB++] = eByte;
  }

  /* NMRA DCC norm ask for two DCC packets instead of only one:
  "Command Stations that generate these packets, and which are not periodically refreshing these functions,
  must send at least two repetitions of these commands when any function state is changed."
  https://www.nmra.org/sites/default/files/s-9.2.1_2012_07.pdf
  */
   sendMessageOnMain( b, nB,1);
 
} // RegisterList::setFunction(ints)


void TPLDCC::setAccessory(int aAdd, int aNum, int activate)  
{
  byte b[3];                      // save space for checksum byte

  b[0] = aAdd % 64 + 128;         // first byte is of the form 10AAAAAA, where AAAAAA represent 6 least significant bits of accessory address  
  b[1] = ((((aAdd / 64) % 8) << 4) + (aNum % 4 << 1) + activate % 2) ^ 0xF8;      // second byte is of the form 1AAACDDD, where C should be 1, and the least significant D represent activate/deactivate

  loadPacket(0, b, 2, 4, 1);

} // RegisterList::setAccessory(ints)


  ///////////////////////////////////////////////////////////////////////////////

bool TPLDCC::writeTextPacket(int nReg, byte *b, int nBytes)  
{
  if (nBytes<2 || nBytes>5) return false;
  sendOnMain(b, nBytes, 1);
}


int TPLDCC::readCVraw(int cv, TPLDCC2 track) 
{
  byte bRead[4];
  int bValue;
  int ret, base;

  cv--;                              // actual CV addresses are cv-1 (0-1023)

  // A read cannot be done if a monitor pin is not defined !
  if (!track.startAckProcess()) return -1;
  
  bRead[0] = 0x78 + (highByte(cv) & 0x03);   // any CV>1023 will become modulus(1024) due to bit-mask of 0x03
  bRead[1] = lowByte(cv);

  bValue = 0;

  for (int i = 0; i<8; i++) {

    
    bRead[2] = 0xE8 + i;

    track.schedulePacket( resetPacket);          // NMRA recommends starting with 3 reset packets
    
    // TODO... which track?
    sendOnPROG(bRead, 3, 5);                // NMRA recommends 5 verify packets
    
    ret = track.getAck();

    bitWrite(bValue, i, ret);
  }

  track.startAckProcess(); // maybe unnecessary

  bRead[0] = 0x74 + (highByte(cv) & 0x03);   // set-up to re-verify entire byte
  bRead[2] = bValue;

  loadPacket(0, resetPacket, 2, 3);          // NMRA recommends starting with 3 reset packets
  loadPacket(0, bRead, 3, 5);                // NMRA recommends 5 verify packets
  loadPacket(0, resetPacket, 2, 1);          // forces code to wait until all repeats of bRead are completed (and decoder begins to respond)

  ret = track.getAck();

  if (ret == 0)    // verify unsuccessful
    bValue = -1;
  return bValue;
}

int TPLDCC::readCV(int cv) 
{
  return readCVraw(cv, TPLDCC1::progTrack);
} 

int TPLDCC::readCVmain(int cv)
{
  return readCVraw(cv,TPLDCC::mainTrack);
} 

///////////////////////////////////////////////////////////////////////////////

void TPLDCC::writeCVByte(int cv, int bValue) 
{
  byte bWrite[4];
  int ret, base;

  cv--;                              // actual CV addresses are cv-1 (0-1023)

  bWrite[0] = 0x7C + (highByte(cv) & 0x03);   // any CV>1023 will become modulus(1024) due to bit-mask of 0x03
  bWrite[1] = lowByte(cv);
  bWrite[2] = bValue;

  loadPacket(0, resetPacket, 2, 1);
  loadPacket(0, bWrite, 3, 4);
  loadPacket(0, resetPacket, 2, 1);
  loadPacket(0, idlePacket, 2, 10);

  // If monitor pin undefined, write cv without any confirmation...
   if (!TPLDCC1::mainTrack.startAckProcess()) return true;

    bWrite[0] = 0x74 + (highByte(cv) & 0x03);   // set-up to re-verify entire byte
    TPLDCC1::scheduleOnMain(resetPacket, 2);          // NMRA recommends starting with 3 reset packets
    sendMessageOnMain(bWrite, 3, 5);               // NMRA recommends 5 verfy packets
    return TPLDCC1::mainTrack.getAck();

} // RegisterList::writeCVByte(ints)


bool TPLDCC::writeCVBit(int cv, int bNum, int bValue) 
{
  byte bWrite[4];
  int ret;

  cv--;                              // actual CV addresses are cv-1 (0-1023)
  bValue = bValue % 2;
  bNum = bNum % 8;

  bWrite[0] = 0x78 + (highByte(cv) & 0x03);   // any CV>1023 will become modulus(1024) due to bit-mask of 0x03
  bWrite[1] = lowByte(cv);
  bWrite[2] = 0xF0 + bValue * 8 + bNum;


  TPLDCC1::scheduleMain(resetPacket,1);
  sendMessageOnMain(bWrite, 3, 4);
  TPLDCC1::scheduleMain(resetPacket,1);
  TPLDCC1::scheduleMain( idlePacket, 10);

  // If monitor pin undefined, write cv without any confirmation...
   if (!TPLDCC1::mainTrack.startAckProcess()) return true;

    bitClear(bWrite[2], 4);                    // change instruction code from Write Bit to Verify Bit
    TPLDCC1::scheduleOnMain(resetPacket, 2);          // NMRA recommends starting with 3 reset packets
    sendMessageOnMain(bWrite, 3, 5);               // NMRA recommends 5 verfy packets
    return TPLDCC1::mainTrack.getAck();
} 


///////////////////////////////////////////////////////////////////////////////

void TPLDCC::writeCVByteMain(int cab, int cv, int bValue) 
{
  byte b[6];                      // save space for checksum byte
  byte nB = 0;

  cv--;

  if (cab>127)
    b[nB++] = highByte(cab) | 0xC0;      // convert train number into a two-byte address

  b[nB++] = lowByte(cab);
  b[nB++] = 0xEC + (highByte(cv) & 0x03);   // any CV>1023 will become modulus(1024) due to bit-mask of 0x03
  b[nB++] = lowByte(cv);
  b[nB++] = bValue;

  sendMessageOnMain(b, nB, 4);

} 

void TPLDCC::writeCVBitMain(int cab, int cv, int bNum, int bValue)  
{
  byte b[6];                      // save space for checksum byte
  byte nB = 0;

  cv--;

  bValue = bValue % 2;
  bNum = bNum % 8;

  if (cab>127)
    b[nB++] = highByte(cab) | 0xC0;      // convert train number into a two-byte address

  b[nB++] = lowByte(cab);
  b[nB++] = 0xE8 + (highByte(cv) & 0x03);   // any CV>1023 will become modulus(1024) due to bit-mask of 0x03
  b[nB++] = lowByte(cv);
  b[nB++] = 0xF0 + bValue * 8 + bNum;

  sendMessageonMain( b, nB, 4);
} 

void TPLDCC::sendMessageOnMain( byte *b, byte nBytes, byte repeats=0) {
  DCCPacket newpacket;
  formatPacket(DCCPacket& newpacket, b, nBytes, 0, repeats );
  TPLDCC1::scheduleMain(newpacket);
}

///////////////////////////////////////////////////////////////////////////////

void TPLDCC::formatPacket(DCCPacket& newpacket, byte *b, byte nBytes, int loco=0 , byte repeats=0 ) {
  newpacket.loco = loco;
  newpacket.repeats = repeats;

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

void TPLDCC::loop() {
   TPLDCC1::loop();
   // TODO... move this stuff to a new Loco table
  // if the main track transmitter still has a pending packet, skip this loop.
  if ( TPLDCC1::mainTrack.packetPending) return;

  // each time around the Arduino loop, we resend a loco speed packet reminder 
  for (; priorityReg < MAX_LOCOS; priorityReg++) {
    if (locoPackets[priorityReg].loco > 0) {
       TPLDCC1::mainTrack.schedulePacket(locoPackets[priorityReg]);
      priorityReg++;
      return;
    }
  }
  for (priorityReg = 0; priorityReg < MAX_LOCOS; priorityReg++) {
    if (locoPackets[priorityReg].loco > 0) {
       TPLDCC1::mainTrack.schedulePacket(locoPackets[priorityReg]);
      priorityReg++;
      return;
    }
  }
 }
 }
