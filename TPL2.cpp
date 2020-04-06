#include "TPL2.h"
#include "TPL.h"
#include <arduino.h>
#include <stdarg.h>
#include <DCCpp.h>
#define DIAG_ENABLED true
#include "DIAG.h"
#include "TPLTurnout.h"
#include "TPLSensors.h"
#include "TPLThrottle.h"
#include "TPLDisplay.h"

const  extern PROGMEM  byte TPLRouteCode[]; // Will be resolved by user creating ROUTES table
TPLDisplay lcddisplay;
bool TPL2::manual_mode=false;
 





int TPL2::locateRouteStart(short _route) {
  if (_route==0) return 0; // Route 0 is always start of ROUTES for default startup 
  for (int pcounter=0;;pcounter+=2) {
    byte opcode=pgm_read_byte_near(TPLRouteCode+pcounter);
    if (opcode==OPCODE_ENDROUTES) return -1;
    if (opcode==OPCODE_ROUTE) if( _route==pgm_read_byte_near(TPLRouteCode+pcounter+1)) return pcounter;
  }
}


autotask* TPL2::tplAddTask2(short _route) {
  autotask* newtask = new autotask();
  newtask->progCounter= locateRouteStart(_route); 
  if (task == NULL) {
    // Create first in ring
    task = newtask;
    newtask->next = task;
  }
  else {
    newtask->next = task->next;
    task->next = newtask;
  }
  return newtask;
}

void TPL2::begin(short _progTrackPin,  // arduino pin connected to progtrack relay
                                     // e.g pin 9 as long as motor shield Brake links cut.
                                     // A relay attached to this pin will switch the programming track
                                     // to become part of the main track.  
                 short _sensors,       // number of sensors on I2C bus
                short _signalZeroPin, // arduino pin connected to first signal
                short _signals,       // Number of signals (2 pins each)
                short _turnouts        // number of turnouts on I2C bus
                ) {
    DIAG(F("TPL begin progtrack=%d,sensors=%d,sig0=%d,sigs=%d,turn=%d\n"),
                      _progTrackPin,_sensors,_signalZeroPin,_signals,_turnouts);
  lcddisplay.begin(16,2);
  lcddisplay.print(F("TPL STARTING"));
  sensorCount=_sensors;              
  TPLSensors::init(_sensors);
  DIAG(F("\nSensors In itialised")); 

  pinMode(_progTrackPin,OUTPUT);
  digitalWrite(_progTrackPin,HIGH);
  progTrackPin=_progTrackPin;

  signalZeroPin = _signalZeroPin;
  for (int pin = 0; pin < _signals + _signals ; pin++) {
    pinMode(pin + signalZeroPin, OUTPUT);
  }
  TPLTurnout::SetTurnouts(_turnouts);
  TPLThrottle::begin();
   tplAddTask2(0); // add the startup route
   DCCpp::begin();
   DCCpp::beginMainMotorShield();
   DCCpp::beginProgMotorShield();
   DCCpp::powerOn();
   lcddisplay.print(F("TPL AUTOMATIC"));
}

bool TPL2::delayme(short csecs) { // returns true if still waiting
  if (task->waitingFor == 0) {
    task->waitingFor = millis() + csecs * 100;
    return true;
  }
  if (task->waitingFor >= millis()) {
   return true;
  }
 task->waitingFor = 0;
  return false;
}

void TPL2::driveLoco(short speedo) {
  if (speedo == task->speedo) return;
  task->speedo = speedo;
   if (task->loco==0) return;
 
  bool direction=task->forward ^task->invert;
  DIAG(F("\nDrive Loco %d speed=%d\% %s"), task->loco, task->speedo, direction ? " Forward" : " Reverse");
  DCCpp::setSpeedMain( task->reg, task->loco, task->locosteps, (int)((task->speedo * task->locosteps) / 100), direction);
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
    task->progCounter += 2;
    short opcode =  pgm_read_byte_near(TPLRouteCode+task->progCounter);;
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

bool TPL2::readLoco() {
  int cv=DCCpp::identifyLocoIdProg();
  DIAG(F("\n READ_LOCO=%d"),cv);
  if (cv>0) {
    task->loco=cv;
    task->locosteps=128; // DCCpp::readCvProg(29) etc
    task->speedo=0;
    task->invert=false;
    return true;
  }
  return false;
 }

 short TPL2::getUnusedReg() {
   for (int reg=1;reg<16;reg++) {
        if (! (flags[reg] & REGISTER_FLAG)) {
           flags[reg] |= REGISTER_FLAG;
           return reg;
          }
   }
   return 0;
 }
 
void TPL2::loop() {
  DCCpp::loop();
  if (task == NULL ) return;
   task = task->next;
  if (task->progCounter < 0) return;
  short opcode = pgm_read_byte_near(TPLRouteCode+task->progCounter);
  short operand =  pgm_read_byte_near(TPLRouteCode+task->progCounter+1);
  // DIAG(F("\npc=%d, Opcd=%d,%d"), task->progCounter, opcode,operand);
  // attention: Returning from this switch leaves the program counter unchanged.
  //            This is used for unfinished waits for timers or sensors.
  //            Breaking from this switch will step to the next step in the route. 
  switch (opcode) {
    case OPCODE_TL:
    case OPCODE_TR:
      if( task->waitingFor>millis()) return;
      task->waitingFor=TPLTurnout::slowSwitch(operand, opcode==OPCODE_TL, task->speedo>0);
      if (task->waitingFor>0) return;
      break; 
    case OPCODE_REV:
      task->forward = false;
      driveLoco(task->speedo);
      break;
    case OPCODE_FWD:
      task->forward = true;
      driveLoco(task->speedo);
      break;
    case OPCODE_SPEED:
      driveLoco(operand);
      break;
    case OPCODE_INVERT_DIRECTION:
      task->invert= !task->invert;
      break;
    case OPCODE_RESERVE:
      if (flags[operand] & SECTION_FLAG) {
        driveLoco(0);
        return;
      }
      DIAG(F("Reserved section"));
      flags[operand] |= SECTION_FLAG;
      break;
    case OPCODE_FREE:
      flags[operand] &= ~SECTION_FLAG;
      break;
    case OPCODE_AT:
      if (readSensor(operand)) break;
      //DIAG(F("\nWAIT %d"),operand);
      return;
    case OPCODE_AFTER: // waits for sensor to hit and then remain off for 0.5 seconds.
      if (readSensor(operand)) {
        task->waitingFor=millis()+500;  // reset timer to half a second
        return;
      }
      if (task->waitingFor == 0 ) return; // not yet reached sensor 
      if (task->waitingFor>millis()) return; // sensor off but not long enough
      task->waitingFor=0; // all done now
      break;
    case OPCODE_SET:
      flags[operand] |= SENSOR_FLAG;
      break;
    case OPCODE_RESET:
      flags[operand] &= ~SENSOR_FLAG;
      break;
    case OPCODE_IF: // do next operand if sensor set
      if (!readSensor(operand)) skipIfBlock();
      break;
    case OPCODE_IFNOT: // do next operand if sensor not set
      if (readSensor(operand)) skipIfBlock();
      break;
   case OPCODE_IFRANDOM: // do next operand on random percentage
      if (random(100)>=operand) skipIfBlock();
      break;
    case OPCODE_ENDIF:
      break;
    case OPCODE_DELAY:
      if (delayme(operand)) return;
      break;
    case OPCODE_RANDWAIT:
      if (delayme((short)random(operand))) return;
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
      task->functions.activate(operand);
      DCCpp::setFunctionsMain(task->reg,task->loco,task->functions);
      break;
    case OPCODE_FOFF:
      task->functions.inactivate(operand);
      DCCpp::setFunctionsMain(task->reg,task->loco,task->functions);
      break;

    case OPCODE_FOLLOW:
      task->progCounter=locateRouteStart(operand);
      return;
    case OPCODE_ENDROUTE:
    case OPCODE_ENDROUTES:
      task->progCounter=-1; // task not in use any more
      flags[task->reg] &= ~REGISTER_FLAG;
      task->loco=0;
      task->reg=0;
      task->invert=false;
      DIAG(F("Task Terminated"));
      return;
    case OPCODE_PROGTRACK:
       DIAG(F("\n progtrack %d pin %d"),operand,progTrackPin);
       if (operand>0) {
        // set progtrack on. drop dccpp register
        flags[task->reg] &= ~REGISTER_FLAG;
        task->reg=0;
        digitalWrite(progTrackPin, HIGH);
       }
       else {
            task->reg=getUnusedReg();
            digitalWrite(progTrackPin, LOW);
       }
       break;
      case OPCODE_READ_LOCO:
       if (!readLoco()) return;
       break;
       case OPCODE_SCHEDULE:
       // Transfer my loco(if any)  to another task and carry on
       // same as create new task at same place as me and carry on!
           {
            autotask* newtask=tplAddTask2(operand);
            newtask->reg=task->reg;
            newtask->loco=task->loco;
            newtask->locosteps=task->locosteps;
            newtask->forward=task->forward;
            newtask->invert=task->invert;
            task->reg=0;
            task->loco=0;
            task->locosteps=0;
            task->invert=false;
       }
           break;
       case OPCODE_SETLOCO:
               {
                // two bytes of loco address are in the next two OPCODE_PAD operands
                int operand2 =  pgm_read_byte_near(TPLRouteCode+task->progCounter+3);
                task->progCounter+=2; // Skip the extra two instructions
                
            task->loco=operand<<7 | operand2;
            task->locosteps=128;
            task->forward=true;
            task->invert=false;
            if (task->reg==0) task->reg=getUnusedReg(); 
            DIAG(F("\n SETLOCO %d reg=%d\n"),task->loco, task->reg);
       }
       break;
       case OPCODE_ROUTE:
          DIAG(F("\n Starting Route %d\n"),operand);
          break;
       case OPCODE_MANUAL:
          if (manual_mode) return; // have to wait for some other task
       DIAG(F("\n Starting MANUAL \n"));
       lcddisplay.clear();
       lcddisplay.setCursor(0,0);
       lcddisplay.print(F("TPL MANUAL "));
       lcddisplay.print(task->loco);
            driveLoco(0);
            lcddisplay.setCursor(0,1);
            lcddisplay.print(task->speedo);
            lcddisplay.print(task->forward?F("  FWD  "):F("  REV  "));
       
            TPLThrottle::zero();
            manual_mode=true;
            break;
       case OPCODE_MANUAL2:
       {
          int counter=TPLThrottle::count();
          //DIAG(F("\nthrottle=%d"),counter);
          if (counter==TPLThrottle::QUIT_MANUAL) {
            lcddisplay.clear();
            lcddisplay.print(F("AUTOMATIC"));
            manual_mode=false;
            break;
          }
          if (counter<0) {
            task->forward = false;
            driveLoco(-counter*4);
          }
          else {
            task->forward = true;
            driveLoco(counter*4);
          }
          lcddisplay.setCursor(0,1);
          lcddisplay.print(task->speedo);
          lcddisplay.print(task->forward?F("  FWD  "):F("  REV  "));
       
          return;
       }
       case OPCODE_PAD:
       break;
    default:
      DIAG(F("\nOpcode not supported\n"));
    }
    task->progCounter+=2;
    }
