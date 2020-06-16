#ifndef TPL_H
  #define TPL_H
  class TPL {
     public:
  
  /**
   * Tells TPL which pins to use for the programming track switch,  sensors and signals.
   Important to note that the sensor and signal pins must not overlap.
   Note also that your routes can refer to sensors beyond sensor max but these will be treated as 
   purely internal to the softwarre. 
   TODO - switch sensors and signals to the I2C bus when the chips arrive from china!! 
   */
  static void begin(short sensors,       // number of sensor pins used
                short signalZeroPin, // arduino pin connected to first signal
                short signals,        // Number of signals (2 pins each)
                short  turnouts        // Number of turnouts 
                );

  // Call this in your loop
  static void loop();
  };
  
enum OPCODE {OPCODE_TL,OPCODE_TR,
             OPCODE_FWD,OPCODE_REV,OPCODE_SPEED,OPCODE_INVERT_DIRECTION,
             OPCODE_RESERVE,OPCODE_FREE,
             OPCODE_AT,OPCODE_AFTER,OPCODE_SET,OPCODE_RESET,
             OPCODE_IF,OPCODE_IFNOT,OPCODE_ENDIF,OPCODE_IFRANDOM,
             OPCODE_DELAY,OPCODE_RANDWAIT,
             OPCODE_FON, OPCODE_FOFF,
             OPCODE_RED,OPCODE_GREEN,
             OPCODE_PAD,OPCODE_STOP,OPCODE_FOLLOW,OPCODE_ENDROUTE,
             OPCODE_PROGTRACK,OPCODE_READ_LOCO1,OPCODE_READ_LOCO2,
             OPCODE_SCHEDULE,OPCODE_SETLOCO,
             OPCODE_PAUSE, OPCODE_RESUME, 
             OPCODE_ROUTE,OPCODE_ENDROUTES
             };

#define ROUTES const  extern PROGMEM  byte TPLRouteCode[] = {
#define ROUTE(id)  OPCODE_ROUTE, id, 
#define ENDROUTE OPCODE_ENDROUTE,0,
#define ENDROUTES OPCODE_ENDROUTES,0 };
 
#define AFTER(sensor_id) OPCODE_AT,sensor_id,OPCODE_AFTER,sensor_id,
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
#define PAUSE OPCODE_PAUSE,0,
#define PROGTRACK(onoff) OPCODE_PROGTRACK,onoff,
#define READ_LOCO OPCODE_READ_LOCO1,0,OPCODE_READ_LOCO2,0,
#define RED(signal_id) OPCODE_RED,signal_id,
#define RESERVE(blockid) OPCODE_RESERVE,blockid,
#define RESET(sensor_id) OPCODE_RESET,sensor_id,
#define RESUME OPCODE_RESUME,0,
#define REV(speed) OPCODE_REV,speed,
#define SCHEDULE(route) OPCODE_SCHEDULE,route,
#define SETLOCO(loco) OPCODE_SETLOCO,loco>>7,OPCODE_PAD,loco&0x7F,
#define SET(sensor_id) OPCODE_SET,sensor_id,
#define SPEED(speed) OPCODE_SPEED,speed,
#define STOP OPCODE_STOP,0, 
#define TL(id)  OPCODE_TL,id,
#define TR(id)  OPCODE_TR,id,
#endif
