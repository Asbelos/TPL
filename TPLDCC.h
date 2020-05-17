

const byte MAIN_POWER_PIN=3;
const byte MAIN_SIGNAL_PIN=12;
const byte MAIN_SENSE_PIN=A0;

const byte PROG_POWER_PIN=11;
const byte PROG_SIGNAL_PIN=13;
const byte PROG_SENSE_PIN=A1;

const int  POWER_SAMPLE_MILLIS=100;
const int  POWER_SAMPLE_MAX=300;

const byte MAX_LOCOS=20;
 
class TPLDCC {
  public:
  static void begin();
  static void loop();
  static void setSpeed(int loco, byte speed, bool forward);
  static void schedulePacketMain(DCCPacket& packet);
  static void schedulePacketProg(DCCPacket& packet);
  static void setProgtrackToMain(bool yes);
  static void formatPacket(DCCPacket& newpacket, byte* b, byte nBytes, int loco , byte repeats );
  static DCCPacket idlePacket;
  
  private:
  static volatile bool progtrackMirror;
  static void interruptHandler();
  static DCCPacket locoPackets[];
  static byte priorityReg;
  static TPLDCC2  mainTrack;
  static TPLDCC2  progTrack;
};
