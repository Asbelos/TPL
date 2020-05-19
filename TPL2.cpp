#include <arduino.h>
#include <stdarg.h>
#include <DIO2.h>
#define DIAG_ENABLED true
#include "TPL2.h"
#include "TPL.h"
#include "TPLDCC2.h"
#include "TPLDCC1.h"
#include "TPLDCC.h"
#include "DIAG.h"
#include "TPLTurnout.h"
#include "TPLSensors.h"
#include "TPLThrottle.h"
#include "TPLDisplay.h"

const GPIO_pin_t flashPin = DP52;//
const GPIO_pin_t powerResetPin = DP40;//


const  extern PROGMEM  byte TPLRouteCode[]; // Will be resolved by user creating ROUTES table
TPLDisplay lcddisplay;
byte TPL2::flags[MAX_FLAGS];
byte TPL2::manualTurnoutNumber=99;  
int TPL2::manualModeCounter=99;  
bool TPL2::manual_mode=false;
bool TPL2::manual_mode_flipflop=false;
int TPL2::signalZeroPin;
byte TPL2::sensorCount;


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

void TPL2::begin(  
                 short _sensors,       // number of sensors on I2C bus
                short _signalZeroPin, // arduino pin connected to first signal
                short _signals,       // Number of signals (2 pins each)
                short _turnouts        // number of turnouts on I2C bus
                ) {

    DIAG(F("TPL begin sensors=%d,sig0=%d,sigs=%d,turn=%d\n"),
                      _sensors,_signalZeroPin,_signals,_turnouts);
   manual_mode=false;
   manual_mode_flipflop=false;
   manualTurnoutNumber=99;
   manualModeCounter=9999;

  lcddisplay.begin(16,2);
  lcddisplay.print(F("TPL STARTING"));
  sensorCount=_sensors;              
  TPLSensors::init(_sensors);
  DIAG(F("\nSensors In itialised"));
  pinMode2f(flashPin,OUTPUT); 
  pinMode2f(powerResetPin,INPUT_PULLUP); 

  signalZeroPin = _signalZeroPin;
  for (int pin = 0; pin < _signals + _signals ; pin++) {
    pinMode(pin + signalZeroPin, OUTPUT);
  }
  TPLTurnout::begin();
  TPLThrottle::begin();
   tplAddTask2(0); // add the startup route
   lcddisplay.clear();
   lcddisplay.print(F("TPL AUTOMATIC"));
   TPLDCC::begin();
 
}

void TPL2::delayme(int millisecs) { 
    task->waitingFor = millis() + millisecs;
}

void TPL2::driveLoco(short speedo) {
  if (speedo == task->speedo) return;
  task->speedo = speedo;
   if (task->loco==0) return;
 
  bool direction=task->forward ^task->invert;
  DIAG(F("\nDrive Loco %d speed=%d\% %s"), task->loco, task->speedo, direction ? " Forward" : " Reverse");
  TPLDCC::setSpeed(task->loco, task->speedo , direction);
  DIAG(F("*OK*\n"));
  }

bool TPL2::doManual() {       
    //DIAG(F("M"));
    if (TPLThrottle::quit()) {
       lcddisplay.setCursor(0,1);
       DIAG(F("\nResume auto"));
       lcddisplay.print(F("AUTO            "));
       manual_mode=false;
       return false;
    }
    
    int counter=TPLThrottle::count();
    //DIAG(F("\nthrottle=%d"),counter);
    if (counter!=manualModeCounter) {
      manualModeCounter=counter;
       if (counter<0) {
          task->forward = false;
          driveLoco(-counter*4);
        }
        else {
          task->forward = true;
          driveLoco(counter*4);
        }
        showManual();
        delayme(200);
        return true; // if speed changed, no need to check keypad just yet
    }
    
    char padKey=TPLThrottle::getKey();  
    if (padKey && padKey!=manualTurnoutNumber) {
              DIAG(F("\nPadKey=%x"),padKey);
            
              if (padKey>='0' && padKey<='9') manualTurnoutNumber=padKey;
              else if (padKey=='*')  TPLTurnout::slowSwitch(manualTurnoutNumber-'0', true,true);
              else if (padKey=='#')  TPLTurnout::slowSwitch(manualTurnoutNumber-'0', false,true);
              delayme(500);  // reset timer to half a second
          }
     return true;
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
  DIAG(F("\nRead Loco\n"));
 
  int cv=0; // DCCpp::identifyLocoIdProg();
  
  
  DIAG(F("\n READ_LOCO=%d"),cv);
  if (cv>0) {
    task->loco=cv;
    task->speedo=0;
    task->invert=false;
    return true;
  }
  return false;
 }

 void TPL2::showManual() {
  lcddisplay.setCursor(0,1);
  lcddisplay.print(F("Manual "));
  lcddisplay.print(task->loco);
  lcddisplay.print(F(" "));
  if (task->speedo==0) lcddisplay.print(F("STOP"));
  else {
     lcddisplay.print(task->speedo);
     lcddisplay.print(task->forward?F(" ->  "):F(" <-  "));
     }
}
void TPL2::showProg(bool progOn) {
  lcddisplay.setCursor(0,0);
  lcddisplay.print(F("P_TRACK         "));
  lcddisplay.setCursor(9,0);
   if (progOn) {
    lcddisplay.print(F("L="));
    lcddisplay.print(task->loco);
   }
   else {
    lcddisplay.print(F("OFF    "));
   }
}

void TPL2::loop2() {
  
  if (task == NULL ) return;
   task = task->next;
  if (task->progCounter < 0) return;
   if (task->waitingFor > millis()) return;
     
  short opcode = pgm_read_byte_near(TPLRouteCode+task->progCounter);
  short operand =  pgm_read_byte_near(TPLRouteCode+task->progCounter+1);
  // DIAG(F("\npc=%d, Opcd=%d,%d"), task->progCounter, opcode,operand);
  // attention: Returning from this switch leaves the program counter unchanged.
  //            This is used for unfinished waits for timers or sensors.
  //            Breaking from this switch will step to the next step in the route. 
  switch (opcode) {
    case OPCODE_TL:
    case OPCODE_TR:
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
      if (!readSensor(operand)) return;
      task->waitAfter=millis()+500;
      break;
    case OPCODE_AFTER: // waits for sensor to hit and then remain off for 0.5 seconds. (must come after an AT operation)
      if (readSensor(operand)) {
        // reset timer to half a second and keep waiting
        task->waitAfter=millis()+500;
        return; 
      }
      if (task->waitAfter>millis()) return;   
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
      delayme(operand*10);
      break;
    case OPCODE_RANDWAIT:
      delayme((short)random(operand*10));
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
      task->progCounter=locateRouteStart(operand);
      return;
    case OPCODE_ENDROUTE:
    case OPCODE_ENDROUTES:
      task->progCounter=-1; // task not in use any more
     task->loco=0;
      task->invert=false;
      DIAG(F("Task Terminated"));
      return;
    case OPCODE_PROGTRACK:
       if (operand>0) {
        TPLDCC1::setProgtrackToMain(false);
        showProg(true);
       }
       else {
            TPLDCC1::setProgtrackToMain(true);
            showProg(false);
       }
       break;
       
      case OPCODE_READ_LOCO:
       if (!readLoco()) return;
       showProg(true);       
       break;
       
       case OPCODE_SCHEDULE:
       // Transfer my loco(if any)  to another task and carry on
       // same as create new task at same place as me and carry on!
           {
            autotask* newtask=tplAddTask2(operand);
            newtask->loco=task->loco;
            newtask->forward=task->forward;
            newtask->invert=task->invert;
            task->loco=0;
            task->invert=false;
       }
           break;
       case OPCODE_SETLOCO:
               {
                // two bytes of loco address are in the next two OPCODE_PAD operands
                int operand2 =  pgm_read_byte_near(TPLRouteCode+task->progCounter+3);
                task->progCounter+=2; // Skip the extra two instructions
                
            task->loco=operand<<7 | operand2;
            task->forward=true;
            task->invert=false;
            DIAG(F("\n SETLOCO %d \n"),task->loco);
       }
       break;
       case OPCODE_ROUTE:
          DIAG(F("\n Starting Route %d\n"),operand);
          break;
       case OPCODE_MANUAL:
          if (manual_mode) return; // have to wait for some other task       
            driveLoco(0);
            TPLThrottle::zero();
            showManual();
            manual_mode=true;
            break; // next opcode will be MANUAL2
       case OPCODE_MANUAL2:
          if (doManual()) return;
          break;
       case OPCODE_PAD:
       break;
    default:
      DIAG(F("\nOpcode not supported\n"));
    }
    task->progCounter+=2;
}
void TPL2::loop() {
    TPLDCC::loop();
    loop2();
}
