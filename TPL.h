#ifndef TPL_H
  #define TPL_H
 
  /**
   * Tells TPL which pins to use for the programming track switch,  sensors and signals.
   Important to note that the sensor and signal pins must not overlap.
   Note also that your routes can refer to sensors beyond sensor max but these will be treated as 
   purely internal to the softwarre. 
   TODO - switch sensors and signals to the I2C bus when the chips arrive from china!! 
   */
  void tplBegin(short progtrackPin,  // arduino pin connected to progtrack relay
                                     // e.g pin 9 as long as motor shield Brake links cut.
                                     // A relay attached to this pin will switch the programming track
                                     // to become part of the main track.  
                short _sensorZeroPin, // arduino pin used for sensor(0) e.g. 22
                short _sensors,       // number of sensor pins used
                short _signalZeroPin, // arduino pin connected to first signal
                short _signals,        // Number of signals (2 pins each)
                short  _turnouts        // Number of turnouts 
                );

  /**
   * Adds a "ROUTE" as a task of things to do, typically withpout a loco. 
   * This may be an animation or something triggered by a sensor.
   * E.g. wait for a button to be pressed then read a loco on the programming track 
   * and send it off along another route.
   */
  void tplAddTask(short _route); 

  // Call this in your loop
  void tplLoop();
  
  
enum OPCODE {OPCODE_TL,OPCODE_TR,
             OPCODE_FWD,OPCODE_REV,OPCODE_SPEED,OPCODE_INVERT_DIRECTION,
             OPCODE_RESERVE,OPCODE_FREE,
             OPCODE_AT,OPCODE_AFTER,OPCODE_SET,OPCODE_RESET,
             OPCODE_IF,OPCODE_IFNOT,OPCODE_ENDIF,OPCODE_IFRANDOM,
             OPCODE_DELAY,OPCODE_RANDWAIT,
             OPCODE_FON, OPCODE_FOFF,
             OPCODE_RED,OPCODE_GREEN,
             OPCODE_PAD,OPCODE_STOP,OPCODE_FOLLOW,OPCODE_ENDROUTE,
             OPCODE_PROGTRACK,OPCODE_READ_LOCO,
             OPCODE_SCHEDULE,OPCODE_SETLOCO,
             OPCODE_ROUTE,OPCODE_ENDROUTES
             };

#define ROUTES const  extern PROGMEM  byte TPLRouteCode[] = {
#define ROUTE(id)  OPCODE_ROUTE, id, 
#define ENDROUTE OPCODE_ENDROUTE,0,
#define ENDROUTES OPCODE_ENDROUTES,0 };
 
#define AFTER(sensor_id) OPCODE_AFTER,sensor_id,
#define AT(sensor_id) OPCODE_AT,sensor_id,
#define DELAY(mindelay) OPCODE_DELAY,mindelay,
#define DELAYRANDOM(mindelay,maxdelay) OPCODE_DELAY,mindelay,OPCODE_RANDWAIT,maxdelay-mindelay,
#define ENDIF  OPCODE_ENDIF,0,
#define FOFF(func) OPCODE_FOFF,func,
#define FOLLOW(route) OPCODE_FOLLOW,route,
#define FON(func) OPCODE_FON,func,
#define FREE(blockid) OPCODE_FREE,blockid,
#define FWD(speed) OPCODE_FWD,speed,
#define GREEN(signal_id) OPCODE_GREEN,signal_id,
#define IF(sensor_id) OPCODE_IF,sensor_id,
#define IFNOT(sensor_id) OPCODE_IFNOT,sensor_id,
#define IFRANDOM(percent) OPCODE_IFRANDOM,percent,
#define INVERT_DIRECTION OPCODE_INVERT_DIRECTION,0,
#define PROGTRACK(onoff) OPCODE_PROGTRACK,onoff,
#define READ_LOCO OPCODE_READ_LOCO,0,
#define RED(signal_id) OPCODE_RED,signal_id,
#define RESERVE(blockid) OPCODE_RESERVE,blockid,
#define RESET(sensor_id) OPCODE_RESET,sensor_id,
#define REV(speed) OPCODE_REV,speed,
#define SCHEDULE(route) OPCODE_SCHEDULE,route,
#define SETLOCO(loco) OPCODE_SETLOCO,loco>>7,OPCODE_PAD,loco&0x7F,
#define SET(sensor_id) OPCODE_SET,sensor_id,
#define SPEED(speed) OPCODE_SPEED,speed,
#define STOP OPCODE_STOP,0, 
#define TL(id)  OPCODE_TL,id,
#define TR(id)  OPCODE_TR,id,
#endif
