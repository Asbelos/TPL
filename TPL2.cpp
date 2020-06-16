#include <arduino.h>
#include <stdarg.h>
#define DIAG_ENABLED true
#include "TPL2.h"
#include "TPL.h"
#include "DCC.h"
#include "DIAG.h"
#include "TPLTurnout.h"
#include "TPLSensors.h"


const  extern PROGMEM  byte TPLRouteCode[]; // Will be resolved by user creating ROUTES table

// Statics 
byte TPL2::flags[MAX_FLAGS];
int TPL2::signalZeroPin;
byte TPL2::sensorCount;
int TPL2::progtrackLocoId;

TPL2 * TPL2::loopTask=NULL; // loopTask contains the address of ONE of the tasks in a ring.
TPL2 * TPL2::pausingTask=NULL; // Task causing a PAUSE. 
 // when pausingTask is set, that is the ONLY task that gets any service,
 // and all others will have their locos stopped, then resumed after the pausing task resumes.

TPL2::TPL2(byte route) {
  progCounter=locateRouteStart(route);
  next=loopTask?:this;
  loopTask=this;
  delayTime=0;
}
TPL2::~TPL2() {
  if (next==this) loopTask=NULL;
  else for (TPL2* ring=next;;ring=ring->next) if (ring->next == this) {
           ring->next=next;
           loopTask=next;
           break;
       }
}


int TPL2::locateRouteStart(short _route) {
  if (_route==0) return 0; // Route 0 is always start of ROUTES for default startup 
  for (int pcounter=0;;pcounter+=2) {
    byte opcode=pgm_read_byte_near(TPLRouteCode+pcounter);
    if (opcode==OPCODE_ENDROUTES) return -1;
    if (opcode==OPCODE_ROUTE) if( _route==pgm_read_byte_near(TPLRouteCode+pcounter+1)) return pcounter;
  }
}

/* static */ void TPL2::begin(  
                 short _sensors,       // number of sensors on I2C bus
                short _signalZeroPin, // arduino pin connected to first signal
                short _signals,       // Number of signals (2 pins each)
                short _turnouts        // number of turnouts on I2C bus
                ) {

    DIAG(F("TPL begin sensors=%d,sig0=%d,sigs=%d,turn=%d\n"),
                      _sensors,_signalZeroPin,_signals,_turnouts);
  sensorCount=_sensors;              
  TPLSensors::init(_sensors);
  DIAG(F("\nSensors In itialised"));
  signalZeroPin = _signalZeroPin;
  for (int pin = 0; pin < _signals + _signals ; pin++) {
    pinMode(pin + signalZeroPin, OUTPUT);
  }
  TPLTurnout::begin();
  TPL2(0); // add the startup route
  DCC::begin();
}

void TPL2::driveLoco(byte speed) {
     if (loco<0) return;  // Caution, allows broadcast! 
     DCC::setThrottle(loco,speed, forward^invert);
     // TODO... if broadcast speed 0 then pause all other tasks. 
}

bool TPL2::readSensor(short id) {
  if (id>=MAX_FLAGS) return false;
  if (flags[id] & SENSOR_FLAG) return true; // sensor locked on by software
  if (id>=sensorCount) return false;           // sensor is software only
  bool s= TPLSensors::readSensor(id); // real hardware sensor
  if (s) DIAG(F("\nSensor %d hit\n"),id);
  return s;
}

void TPL2::skipIfBlock() {
  short nest = 1;
  while (nest > 0) {
    progCounter += 2;
    byte opcode =  pgm_read_byte_near(TPLRouteCode+progCounter);;
    switch(opcode) {
      case OPCODE_IF:
      case OPCODE_IFNOT:
      case OPCODE_IFRANDOM:
           nest++;
           break;
      case OPCODE_ENDIF:
           nest--;
           break;
      default:
      break;
    }
  }
}

void TPL2::setSignal(short num, bool go) {
  // DIAG(" Signal %d %s", num, go ? "Green" : "Red");
  int pinRed = signalZeroPin + num + num;
  digitalWrite(pinRed, go ? HIGH : LOW);
  digitalWrite(pinRed + 1, go ? LOW : HIGH);
}

/* static */ void TPL2::readLocoCallback(int cv) {
     progtrackLocoId=cv;
}

void TPL2::loop() {
     DCC::loop();
     // Round Robin call to a TPL task each time 
     if (!loopTask) return; 
     loopTask=loopTask->next;
     if (pausingTask==NULL || pausingTask==loopTask) loopTask->loop2();
}    

  
void TPL2::loop2() {
   if (delayTime && millis()-delayStart <delayTime) return;
     
  byte opcode = pgm_read_byte_near(TPLRouteCode+progCounter);
  byte operand =  pgm_read_byte_near(TPLRouteCode+progCounter+1);
   
  // Attention: Returning from this switch leaves the program counter unchanged.
  //            This is used for unfinished waits for timers or sensors.
  //            Breaking from this switch will step to the next step in the route. 
  switch (opcode) {
    
    case OPCODE_TL:
    case OPCODE_TR:
         TPLTurnout::slowSwitch(operand, opcode==OPCODE_TL, true);
         break; 
    
    case OPCODE_REV:
      forward = false;
      driveLoco(speedo);
      break;
    
    case OPCODE_FWD:
      forward = true;
      driveLoco(speedo);
      break;
      
    case OPCODE_SPEED:
      driveLoco(operand);
      break;
    
    case OPCODE_INVERT_DIRECTION:
      invert= !invert;
      break;
      
    case OPCODE_RESERVE:
      if (flags[operand] & SECTION_FLAG) {
        driveLoco(0);
        return;
      }
      flags[operand] |= SECTION_FLAG;
      break;
    
    case OPCODE_FREE:
      flags[operand] &= ~SECTION_FLAG;
      break;
    
    case OPCODE_AT:
      if (!readSensor(operand)) return;
      delayTime=50;
      break;
    
    case OPCODE_AFTER: // waits for sensor to hit and then remain off for 0.5 seconds. (must come after an AT operation)
      if (readSensor(operand)) {
        // reset timer to half a second and keep waiting
        waitAfter=millis();
        return; 
      }
      if (millis()-waitAfter < 500 ) return;   
      break;
    
    case OPCODE_SET:
      flags[operand] |= SENSOR_FLAG;
      break;
    
    case OPCODE_RESET:
      flags[operand] &= ~SENSOR_FLAG;
      break;

    case OPCODE_PAUSE:
         DCC::setThrottle(0,0,true);  // pause all locos on the track
         pausingTask=this;
         break;
 
    case OPCODE_RESUME:
         pausingTask=NULL;
         for (TPL2 * t=next; t!=this;t=t->next) if (t->loco >0) t->driveLoco(t->speedo);
          break;        
    
    case OPCODE_IF: // do next operand if sensor set
      if (!readSensor(operand)) skipIfBlock();
      break;
    
    case OPCODE_IFNOT: // do next operand if sensor not set
      if (readSensor(operand)) skipIfBlock();
      break;
   
    case OPCODE_IFRANDOM: // do block on random percentage
      if (random(100)>=operand) skipIfBlock();
      break;
    
    case OPCODE_ENDIF:
      break;
    
    case OPCODE_DELAY:
      delayMe(operand*10);
      break;
    
    case OPCODE_RANDWAIT:
      delayMe((int)random(operand*10));
      break;
    
    case OPCODE_RED:
      setSignal(operand,false);
      break;
    
    case OPCODE_GREEN:
      setSignal(operand,true);
      break;
    
    case OPCODE_STOP:
      driveLoco(0);
      break;
    
    case OPCODE_FON:
      // task->functions.activate(operand);
      // DCCpp::setFunctionsMain(task->reg,task->loco,task->functions);
      break;
    
    case OPCODE_FOFF:
      // task->functions.inactivate(operand);
      // DCCpp::setFunctionsMain(task->reg,task->loco,task->functions);
      break;

    case OPCODE_FOLLOW:
      progCounter=locateRouteStart(operand);
      if (progCounter<0) delete this; 
      return;
      
    case OPCODE_ENDROUTE:
    case OPCODE_ENDROUTES:
      delete this;  // removes this task from the ring buffer
      return;
      
    case OPCODE_PROGTRACK:
       if (operand>0) {
        // TODO TPLDCC1::setProgtrackToMain(false);
        // showProg(true);
       }
       else {
            // TODO TPLDCC1::setProgtrackToMain(true);
            // showProg(false);
       }
       break;
       
      case OPCODE_READ_LOCO1:
       progtrackLocoId=-1;
       DCC::getLocoId(readLocoCallback);
       break;
      
      case OPCODE_READ_LOCO2:
       if (progtrackLocoId<0) return; // still waiting for callback
       loco=progtrackLocoId;
       speedo=0;
       forward=true;
       invert=false;
       break;
       
       case OPCODE_SCHEDULE:
           {
            // Create new task and transfer loco.....
            // but cheat by swapping prog counters with new task  
            new TPL2(operand);
            int swap=loopTask->progCounter;
            loopTask->progCounter=progCounter+2;
            progCounter=swap;
           }
           break;
       
       case OPCODE_SETLOCO:
           {
            // two bytes of loco address are in the next two OPCODE_PAD operands
             int operand2 =  pgm_read_byte_near(TPLRouteCode+progCounter+3);
             progCounter+=2; // Skip the extra two instructions
             loco=operand<<7 | operand2;
             speedo=0;
             forward=true;
             invert=false;
             DIAG(F("\n SETLOCO %d \n"),loco);
            }
       break;
       
       case OPCODE_ROUTE:
          DIAG(F("\n Starting Route %d\n"),operand);
          break;

       case OPCODE_PAD:
          // Just a padding for previous opcode needing >1 operad byte.
       break;
    
    default:
      DIAG(F("\nOpcode %d not supported\n"),opcode);
    }
    // Falling out of the switch means move on to the next opcode
    progCounter+=2;
}

void TPL2::delayMe(int delay) {
     delayTime=delay;
     delayStart=millis();
}
