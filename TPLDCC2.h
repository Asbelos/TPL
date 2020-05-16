class DCCPacket {
  public:
       int loco;
       byte bits;
       byte repeats;
       byte bits_sent;
       byte data[10];
};
#include <Arduino.h>
 
class TPLDCC2 {
  public:
   TPLDCC2(byte powerPin, byte signalPin,byte sensePin);
  void checkPowerOverload();
  bool interrupt1();
  void interrupt2();
  void schedulePacket(DCCPacket packet);
   volatile bool packetPending;
  private:
  DCCPacket transmitPacket;
  DCCPacket pendingPacket;
  byte state;
  bool currentBit;  
  GPIO_pin_t directionPin;
  GPIO_pin_t powerPin;
 
  // current sampling 
  byte sensePin;
  int smoothedCurrent;
  unsigned long nextSampleDue;
};
