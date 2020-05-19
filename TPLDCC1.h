#include <DIO2.h>




// This hardware configuration would normally be setup in a .h file elsewhere
const byte MAIN_POWER_PIN=3;
const byte MAIN_SIGNAL_PIN=12;
const byte MAIN_SENSE_PIN=A0;

const byte PROG_POWER_PIN=11;
const byte PROG_SIGNAL_PIN=13;
const byte PROG_SENSE_PIN=A1;

const int  POWER_SAMPLE_MAX=300;
const int  POWER_SAMPLE_ON_WAIT=100;
const int  POWER_SAMPLE_OFF_WAIT=1000;
const int  POWER_SAMPLE_OVERLOAD_WAIT=4000;

// NOTE: static functions are used for the overall controller, then 
// one instance is created for each track.

struct DCCPacket {
       byte bits;
       byte repeats;
       byte data[10];
};

enum class POWERMODE { OFF, ON, OVERLOAD };

class TPLDCC1 {
  public:
   static void begin();
   static void loop();
   static TPLDCC2  mainTrack;
   static TPLDCC2  progTrack; 

  void beginTrack();
  void setPowerMode(POWERMODE);
  POWERMODE getPowerMode();
  void checkPowerOverload();
  bool interrupt1();
  void interrupt2();
  void schedulePacket(DCCPacket& packet);
  volatile bool packetPending;
  bool  startAckProcess();
  bool  getAck();

  
  private:
  static void interruptHandler();
  POWERMODE powerMode;
  DCCPacket transmitPacket;
  DCCPacket pendingPacket;
  byte bits_sent;
  byte state;
  bool currentBit;  
  GPIO_pin_t directionPin;
  GPIO_pin_t powerPin;
 
  // current sampling 
  bool isMainTrack;
  byte sensePin;
  unsigned long nextSampleDue;
  
};
