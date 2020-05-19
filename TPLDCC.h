

const byte MAX_LOCOS=20;


class TPLDCC {
  public:
  static void begin();
  static void loop();
  static void setSpeed(int loco, byte speed, bool forward);
  static int  readCV(int cv);
  static int  readCVMain(int cv);
  static bool writeCVByte(int cv, int bValue) ;
  static bool writeCVBit(int cv, int bNum, int bValue);
  static void writeCVByteMain(int cab, int cv, int bValue);
  static void writeCVBitMain(int cab, int cv, int bNum, int bValue);
  static void setFunction( int cab, int fByte, int eByte);
  static void setAccessory(int aAdd, int aNum, int activate) ;
  static void writeTextPacket(int nReg, byte *b, int nBytes);
  static DCCPacket idlePacket;

  private: 
  static int readCVraw(int cv, TPLDCC2 track); 
  static void formatPacket(DCCPacket& newpacket, byte* b, byte nBytes, int loco , byte repeats );
  static void sendMessageOnMain( byte *b, byte nBytes, byte repeats=0);
  
  static byte priorityReg;
 static DCCPacket locoPackets[MAX_LOCOS];
  
};
