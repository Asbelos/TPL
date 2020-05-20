

const byte MAX_LOCOS=20;

struct LOCO {
       int loco;
       byte speed;
       bool forward;
};




class TPLDCC {
  public:
  static void begin();
  static void loop();

  // DCC API functions
  static void setSpeed(int loco, byte speed, bool forward, bool isReminder=false);
  static int  readCV(int cv);
  static bool writeCVByte(int cv, int bValue) ;
  static bool writeCVBit(int cv, int bNum, int bValue);
  static void writeCVByteMain(int cab, int cv, int bValue);
  static void writeCVBitMain(int cab, int cv, int bNum, int bValue);
  static void setFunction( int cab, int fByte, int eByte);
  static void setAccessory(int aAdd, int aNum, int activate) ;
  static bool writeTextPacket( byte *b, int nBytes);

private: 
  static void  updateLocoReminder(int loco, byte tSpeed, bool forward);
  static LOCO speedTable[MAX_LOCOS];
  static byte nextLoco;
  
};
