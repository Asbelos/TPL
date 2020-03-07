#ifndef TPL_H
  #define TPL_H
  void tplAddRoutes(const short * _routes[]);
  void tplAddTask(const short* _route);
  void tplAddJourney(const short* _route, short _reg, short _loco, short _steps);
  void tplSensorZeroPin(short _sensorZeroPin,short _sensors);
  void tplSignalsZeroPin(short _signalZeroPin,short _signals);
  void tplProgTrackPin(short _pin);
  void tplLoop();
  
  
enum OPCODE {OPCODE_TL,OPCODE_TR,
             OPCODE_FWD,OPCODE_REV,OPCODE_SPEED,
             OPCODE_RESERVE,OPCODE_FREE,
             OPCODE_AT,OPCODE_NOTAT,OPCODE_SET,OPCODE_RESET,
             OPCODE_IF,OPCODE_IFNOT,OPCODE_ENDIF,
             OPCODE_DELAY,OPCODE_RANDWAIT,
             OPCODE_RED,OPCODE_GREEN,
             OPCODE_PAD,OPCODE_STOP,OPCODE_AGAIN,OPCODE_FOLLOW,OPCODE_ENDPROG,
             OPCODE_PROGTRACK
             };


#define ROUTE(name) const  PROGMEM  short name[] = {
#define TL(id)  OPCODE_TL,id,
#define TR(id)  OPCODE_TR,id,
#define SPEED(speed) OPCODE_SPEED,speed,
#define FWD(speed) OPCODE_FWD,speed,
#define REV(speed) OPCODE_REV,speed,
#define RESERVE(blockid) OPCODE_RESERVE,blockid,
#define FREE(blockid) OPCODE_FREE,blockid,
#define AT(sensor_id) OPCODE_AT,sensor_id,
#define NOTAT(sensor_id) OPCODE_NOTAT,sensor_id,
#define IF(sensor_id) OPCODE_IF,sensor_id,
#define IFNOT(sensor_id) OPCODE_IFNOT,sensor_id,
#define ENDIF(sensor_id) OPCODE_ENDIF,sensor_id,
#define RED(signal_id) OPCODE_RED,signal_id,
#define GREEN(signal_id) OPCODE_GREEN,signal_id,
#define SET(sensor_id) OPCODE_SET,sensor_id,
#define RESET(sensor_id) OPCODE_RESET,sensor_id,
#define DELAY(mindelay,maxdelay) OPCODE_DELAY,mindelay,OPCODE_RANDWAIT,maxdelay-mindelay,
#define FOLLOW(route) OPCODE_FOLLOW,route,
#define STOP OPCODE_STOP,0, 
#define AGAIN OPCODE_AGAIN,0, 
#define PROGTRACK(onoff) OPCODE_PROGTRACK,onoff,
#define ENDPROG OPCODE_ENDPROG,0, 
#endif
