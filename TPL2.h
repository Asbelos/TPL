 #include "autotask.h"
 class TPL2 {
   public:
    int locateRouteStart(short _route);
    autotask* tplAddTask2(short _route);
    void begin(short _sensors,       // number of sensors on I2C bus
                short _signalZeroPin, // arduino pin connected to first signal
                short _signals,       // Number of signals (2 pins each)
                short _turnouts        // number of turnouts on I2C bus
                );
  void delayme(int millisecs);
  void driveLoco(short speedo);
  bool readSensor(short id);
  void skipIfBlock();
  void setSignal(short num, bool go);
  bool readLoco();
  short getUnusedReg();
  void showManual();
  void showProg(bool progOn);
  bool doManual();
  void loop();
   private:    
  void loop2();          
   autotask* task = NULL;
  static const short SECTION_FLAG = 0x01;
  static const short SENSOR_FLAG = 0x02;
  static const short SIGNAL_FLAG = 0x04;
  static const short REGISTER_FLAG = 0x08;
  static const short MAX_FLAGS=128;
  static byte flags[MAX_FLAGS];
  static byte sensorCount;
  static bool manual_mode;
  static bool manual_mode_flipflop;
  static byte manualTurnoutNumber;
  static int  manualModeCounter;
  static int signalZeroPin;

};
