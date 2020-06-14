 class TPL2 {
   public:
    static void begin(short _sensors,       // number of sensors on I2C bus
                short _signalZeroPin, // arduino pin connected to first signal
                short _signals,       // Number of signals (2 pins each)
                short _turnouts        // number of turnouts on I2C bus
                );
    static void loop();
    TPL2(byte route);
    ~TPL2();
    static void readLocoCallback(int cv);
  private: 
    static int locateRouteStart(short _route);
    static int progtrackLocoId;
    static TPL2 * loopTask;
    void delayme(int millisecs);
    void driveLoco(short speedo);
    bool readSensor(short id);
    void skipIfBlock();
    void setSignal(short num, bool go);
    bool readLoco();
    void showManual();
    void showProg(bool progOn);
    bool doManual();
    void loop2();          
   
  static const short SECTION_FLAG = 0x01;
  static const short SENSOR_FLAG = 0x02;
  static const short SIGNAL_FLAG = 0x04;
  static const short MAX_FLAGS=128;
  static byte flags[MAX_FLAGS];
  static byte sensorCount;
  static bool manual_mode;
  static bool manual_mode_flipflop;
  static byte manualTurnoutNumber;
  static int  manualModeCounter;
  static int signalZeroPin;

 // Local variables
    int progCounter;    // Byte offset of next route opcode in ROUTES table
    unsigned long waitingFor; // Used by opcodes that must be recalled before completing
    unsigned long waitAfter; // Used by OPCODE_AFTER
    int loco;
    bool forward;
    bool invert;
    int speedo;
    TPL2 *next;   // loop chain 
};
