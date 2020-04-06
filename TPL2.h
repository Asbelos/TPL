 #include "autotask.h"
 class TPL2 {
   public:
    int locateRouteStart(short _route);
    autotask* tplAddTask2(short _route);
    void begin(short _progTrackPin,  // arduino pin connected to progtrack relay
                                     // e.g pin 9 as long as motor shield Brake links cut.
                                     // A relay attached to this pin will switch the programming track
                                     // to become part of the main track.  
                 short _sensors,       // number of sensors on I2C bus
                short _signalZeroPin, // arduino pin connected to first signal
                short _signals,       // Number of signals (2 pins each)
                short _turnouts        // number of turnouts on I2C bus
                );
  bool delayme(short csecs);
  void driveLoco(short speedo);
  bool readSensor(short id);
  void skipIfBlock();
  void setSignal(short num, bool go);
  bool readLoco();
  short getUnusedReg();
  void loop();
   private:             
   autotask* task = NULL;
static const short SECTION_FLAG = 0x01;
static const short SENSOR_FLAG = 0x02;
static const short SIGNAL_FLAG = 0x04;
static const short REGISTER_FLAG = 0x08;
static const short MAX_FLAGS=128;
 short flags[MAX_FLAGS];
 short sensorCount;
 short progTrackPin;
 short signalZeroPin;
 static bool manual_mode;
};
