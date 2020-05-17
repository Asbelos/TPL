struct DCCPacket {
       int loco;
       byte bits;
       byte repeats;
       byte bits_sent;
       byte data[10];
};
 
class TPLDCC2 {
  public:
   TPLDCC2(byte powerPin, byte signalPin,byte sensePin);
  void begin();
  void checkPowerOverload();
  bool interrupt1();
  void interrupt2();
  void schedulePacket(DCCPacket& packet);
  volatile bool packetPending;
  void mirror(TPLDCC2 otherTrack);
  
  private:
  DCCPacket transmitPacket;
  DCCPacket pendingPacket;
  byte state;
  bool currentBit;  
  GPIO_pin_t directionPin;
  GPIO_pin_t powerPin;
 
  // current sampling 
  byte sensePin;
  unsigned long nextSampleDue;
};
