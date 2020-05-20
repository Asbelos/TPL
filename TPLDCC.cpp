
#include <DIO2.h>
#include "TPLDCC1.h"
#include "TPLDCC.h"
#include "DIAG.h"



const byte idleMessage[]={0xFF,0x00};
const byte resetMessage[]={0x00,0x00};

byte TPLDCC::nextLoco=0;  // position of loop in loco speed refresh cycle
LOCO TPLDCC::speedTable[MAX_LOCOS];
 
void TPLDCC::begin() {
  TPLDCC1::begin();
}

 void TPLDCC::setSpeed(int loco, byte tSpeed, bool forward, bool isReminder) {
  // create speed msg
  byte b[5];                      // save space for checksum byte
  byte nB = 0;

  if (loco > 127)
    b[nB++] = highByte(loco) | 0xC0;      // convert train number into a two-byte address

  b[nB++] = lowByte(loco);
  b[nB++] = 0x3F;                        // 128-step speed control byte
  b[nB++] = tSpeed + (tSpeed > 0) + forward * 128; // max speed is 126, but speed codes range from 2-127 (0=stop, 1=emergency stop)

   TPLDCC1::mainTrack.schedulePacket( b, nB,0);
  if (!isReminder) updateLocoReminder( loco,  tSpeed,  forward);
 }

 void  TPLDCC::updateLocoReminder(int loco, byte tSpeed, bool forward) {
  // determine speed reg for this loco
  int reg;
  int firstEmpty=MAX_LOCOS;
  for (reg = 0; reg < MAX_LOCOS; reg++) {
    if (speedTable[reg].loco==loco) break;
    if (speedTable[reg].loco==0 && firstEmpty==MAX_LOCOS) firstEmpty=reg;
  }
  if (reg==MAX_LOCOS) reg=firstEmpty; 
  if (reg >= MAX_LOCOS) {
    DIAG(F("\nToo many locos\n"));
    return;
  }
  speedTable[reg].loco=loco;
  speedTable[reg].speed=tSpeed;
  speedTable[reg].forward=forward;
  nextLoco = reg;
  }
   
void TPLDCC::setFunction( int cab, int fByte, int eByte) 
{
 byte b[4];              
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
    TPLDCC1::mainTrack.schedulePacket( b, nB,1);
 
} // RegisterList::setFunction(ints)


void TPLDCC::setAccessory(int aAdd, int aNum, int activate)  
{
  byte b[2]; 

  b[0] = aAdd % 64 + 128;         // first byte is of the form 10AAAAAA, where AAAAAA represent 6 least significant bits of accessory address  
  b[1] = ((((aAdd / 64) % 8) << 4) + (aNum % 4 << 1) + activate % 2) ^ 0xF8;      // second byte is of the form 1AAACDDD, where C should be 1, and the least significant D represent activate/deactivate

   TPLDCC1::mainTrack.schedulePacket( b,2,0);

} 


  ///////////////////////////////////////////////////////////////////////////////

bool TPLDCC::writeTextPacket( byte *b, int nBytes)  
{
  if (nBytes<2 || nBytes>5) return false;
   TPLDCC1::mainTrack.schedulePacket(b, nBytes,0);
  return true;
}


int TPLDCC::readCV(int cv) 
{
  byte bRead[4];
  int bValue;
  int ret;

  cv--;                              // actual CV addresses are cv-1 (0-1023)

  // A read cannot be done if a monitor pin is not defined !
  if (!TPLDCC1::progTrack.startAckProcess()) return -1;
  
  bRead[0] = 0x78 + (highByte(cv) & 0x03);   // any CV>1023 will become modulus(1024) due to bit-mask of 0x03
  bRead[1] = lowByte(cv);

  bValue = 0;

  for (int i = 0; i<8; i++) {

    
    bRead[2] = 0xE8 + i;

    TPLDCC1::progTrack.schedulePacket( resetMessage,2,2);          // NMRA recommends starting with 3 reset packets
    
    // TODO... which track?
    TPLDCC1::progTrack.schedulePacket(bRead, 3, 5);                // NMRA recommends 5 verify packets
    
    ret = TPLDCC1::progTrack.getAck();

    bitWrite(bValue, i, ret);
  }

  TPLDCC1::progTrack.startAckProcess(); // maybe unnecessary

  bRead[0] = 0x74 + (highByte(cv) & 0x03);   // set-up to re-verify entire byte
  bRead[2] = bValue;

  TPLDCC1::progTrack.schedulePacket(resetMessage, 2,2);          // NMRA recommends starting with 3 reset packets
  TPLDCC1::progTrack.schedulePacket(bRead, 3, 5);                // NMRA recommends 5 verify packets
  TPLDCC1::progTrack.schedulePacket(resetMessage, 2, 2);           // forces code to wait until all repeats of bRead are completed (and decoder begins to respond)
  while(TPLDCC1::progTrack.packetPending) delay(1);            // waits until sender has started sending the reset
  
  ret = TPLDCC1::progTrack.getAck();

  if (ret == 0)    // verify unsuccessful
    bValue = -1;
  return bValue;
}


///////////////////////////////////////////////////////////////////////////////

bool TPLDCC::writeCVByte(int cv, int bValue) 
{
  byte bWrite[3];

  cv--;                              // actual CV addresses are cv-1 (0-1023)

  bWrite[0] = 0x7C + (highByte(cv) & 0x03);   // any CV>1023 will become modulus(1024) due to bit-mask of 0x03
  bWrite[1] = lowByte(cv);
  bWrite[2] = bValue;

  TPLDCC1::progTrack.schedulePacket( resetMessage,2, 1);
  TPLDCC1::progTrack.schedulePacket(bWrite, 3, 4);
  TPLDCC1::progTrack.schedulePacket( resetMessage,2, 1);
  TPLDCC1::progTrack.schedulePacket( idleMessage,2, 1);
  while(TPLDCC1::progTrack.packetPending) delay(1);
  
  // If monitor pin undefined, write cv without any confirmation...
   if (!TPLDCC1::mainTrack.startAckProcess()) return true;

    bWrite[0] = 0x74 + (highByte(cv) & 0x03);   // set-up to re-verify entire byte
    TPLDCC1::mainTrack.schedulePacket(resetMessage, 2, 2);          // NMRA recommends starting with 3 reset packets
     TPLDCC1::mainTrack.schedulePacket(bWrite, 3, 5);               // NMRA recommends 5 verfy packets
    return TPLDCC1::mainTrack.getAck();

}  


bool TPLDCC::writeCVBit(int cv, int bNum, int bValue) 
{
  byte bWrite[3];

  cv--;                              // actual CV addresses are cv-1 (0-1023)
  bValue = bValue % 2;
  bNum = bNum % 8;

  bWrite[0] = 0x78 + (highByte(cv) & 0x03);   // any CV>1023 will become modulus(1024) due to bit-mask of 0x03
  bWrite[1] = lowByte(cv);
  bWrite[2] = 0xF0 + bValue * 8 + bNum;


  TPLDCC1::progTrack.schedulePacket(resetMessage,2,1);
  TPLDCC1::progTrack.schedulePacket(bWrite, 3, 4);
  TPLDCC1::progTrack.schedulePacket(resetMessage,2, 1);
  TPLDCC1::progTrack.schedulePacket( idleMessage,2, 10);

  // If monitor pin undefined, write cv without any confirmation...
   if (!TPLDCC1::progTrack.startAckProcess()) return true;

  bitClear(bWrite[2], 4);                    // change instruction code from Write Bit to Verify Bit
  TPLDCC1::progTrack.schedulePacket(resetMessage, 2,2);          // NMRA recommends starting with 3 reset packets
  TPLDCC1::progTrack.schedulePacket(bWrite, 3, 4);               // NMRA recommends 5 verfy packets
  return TPLDCC1::progTrack.getAck();
} 


///////////////////////////////////////////////////////////////////////////////

void TPLDCC::writeCVByteMain(int cab, int cv, int bValue) 
{
  byte b[6];                     
  byte nB = 0;

  cv--;

  if (cab>127)
    b[nB++] = highByte(cab) | 0xC0;      // convert train number into a two-byte address

  b[nB++] = lowByte(cab);
  b[nB++] = 0xEC + (highByte(cv) & 0x03);   // any CV>1023 will become modulus(1024) due to bit-mask of 0x03
  b[nB++] = lowByte(cv);
  b[nB++] = bValue;

  TPLDCC1::mainTrack.schedulePacket(b, nB, 4);

} 

void TPLDCC::writeCVBitMain(int cab, int cv, int bNum, int bValue)  
{
  byte b[6];                     
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

   TPLDCC1::mainTrack.schedulePacket( b, nB, 4);
} 

////////////////////////////////////////////////////////// /////////////////////


void TPLDCC::loop() {
   TPLDCC1::loop();
   // if the main track transmitter still has a pending packet, skip this loop.
  if ( TPLDCC1::mainTrack.packetPending) return;

  // each time around the Arduino loop, we resend a loco speed packet reminder 
  for (; nextLoco < MAX_LOCOS; nextLoco++) {
    if (speedTable[nextLoco].loco > 0) {
       setSpeed(speedTable[nextLoco].loco, speedTable[nextLoco].speed, speedTable[nextLoco].forward, true);
       nextLoco++;
      return;
    }
  }
  for (nextLoco = 0; nextLoco < MAX_LOCOS; nextLoco++) {
    if (speedTable[nextLoco].loco > 0) {
      setSpeed(speedTable[nextLoco].loco, speedTable[nextLoco].speed, speedTable[nextLoco].forward, true);
      nextLoco++;
      return;
    }
  }
 }
 
