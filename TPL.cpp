#include "TPL.h"
#include "autotask.h"
#include <arduino.h>
#include <stdarg.h>
#include <DCCpp.h>
#define DIAG_ENABLED true
#include "DIAG.h"
#include "TPLTurnout.h"


autotask* task = NULL;
const short SECTION_FLAG = 0x01;
const short SENSOR_FLAG = 0x02;
const short SIGNAL_FLAG = 0x04;
const short REGISTER_FLAG = 0x08;

short flags[64];
const short *tplRoutes;

short sensorZeroPin;
short sensorMax;

void tplSensorZeroPin(short _sensorZeroPin, short _sensors) {
  sensorZeroPin = _sensorZeroPin;
  sensorMax=_sensors-1;
  for (int pin = 0; pin < _sensors; pin++) {
    pinMode(pin + sensorZeroPin, INPUT_PULLUP);
  }
}
short progTrackPin;
void tplProgTrackPin(short _pin) {
  pinMode(_pin,OUTPUT);
  digitalWrite(_pin,HIGH);
  progTrackPin=_pin;
}

short signalZeroPin;
void tplSignalsZeroPin(short _signalZeroPin, short _signals) {
  signalZeroPin = _signalZeroPin;
  for (int pin = 0; pin < _signals + _signals ; pin++) {
    pinMode(pin + signalZeroPin, OUTPUT);
  }
}



void tplAddRoutes(const short _routes[]) {
  tplRoutes = _routes;
}

void tplAddTask(short _route) {
  tplAddJourney(_route, 0, 0, 0);
}

int locateRouteStart(short _route) {
  for (int pcounter=0;;pcounter+=2) {
    byte opcode=pgm_read_byte_near(tplRoutes+pcounter);
    if (opcode==OPCODE_ENDROUTES) return -1;
    if (opcode==OPCODE_ROUTE) if( _route==pgm_read_byte_near(tplRoutes+pcounter+1)) return pcounter;
  }
}
void tplAddJourney(short _route, short _reg, short _loco, short _steps) {
  DIAG("\nAddTask Loco=%d\n", _loco);
  autotask* newtask = new autotask();
  newtask->loco = _loco;
  newtask->locosteps = _steps;
  newtask->reg = _reg;
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
}

bool delayme(short csecs) { // returns true if still waiting
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

void driveLoco(short speedo) {
  if (speedo == task->speedo) return;
  task->speedo = speedo;
  DIAG("\nDrive Loco %d speed=%d\% %s", task->loco, task->speedo, task->forward ? " Forward" : " Reverse");
  DCCpp::setSpeedMain( task->reg, task->loco, task->locosteps, (int)((task->speedo * task->locosteps) / 100), task->forward);
}



bool readSensor(short id) {
  if (flags[id] & SENSOR_FLAG) return true; // sensor locked on by software
  if (id>sensorMax) return false;           // sensor is software only
  return digitalRead(sensorZeroPin + id) == LOW; // real hardware sensor
}

void skipIfBlock() {
  short nest = 1;
  while (nest > 0) {
    task->progCounter += 2;
    short opcode =  pgm_read_byte_near(tplRoutes+task->progCounter);;
    if (opcode == OPCODE_IF) nest++;
    if (opcode == OPCODE_IFNOT) nest++;
    if (opcode == OPCODE_ENDIF) nest--;
  }
}

void setSignal(short num, bool go) {
  // DIAG(" Signal %d %s", num, go ? "Green" : "Red");
  int pinRed = signalZeroPin + num + num;
  digitalWrite(pinRed, go ? HIGH : LOW);
  digitalWrite(pinRed + 1, go ? LOW : HIGH);
}

bool readLoco() {
  int cv=DCCpp::identifyLocoIdProg();
  DIAG("\n READ_LOCO=%d",cv);
  if (cv>0) {
    task->loco=cv;
    task->locosteps=128; // DCCpp::readCvProg(29) etc
    task->speedo=0;
    
    DIAG(" STEPS=%d",task->locosteps);
    return true;
  }
  return false;
 }
 
void tplLoop() {
  if (task == NULL ) return;
   task = task->next;
  if (task->progCounter < 0) return;
  short opcode = pgm_read_byte_near(tplRoutes+task->progCounter);
  short operand =  pgm_read_byte_near(tplRoutes+task->progCounter+1);
  // DIAG("\nSTEP Loco=%d prog=%d opcode=%d operand=%d ", task->loco, task->progCounter, opcode, operand);
  switch (opcode) {
    case OPCODE_TL:
    case OPCODE_TR:
      if (delayme(1)) return;
      if (!TPLTurnout::slowSwitch(operand, opcode==OPCODE_TL, task->speedo>0)) return;
      break;
    case OPCODE_FWD:
      task->forward = true;
      driveLoco(operand);
      break;
    case OPCODE_REV:
      task->forward = false;
      driveLoco(operand);
      break;
    case OPCODE_SPEED:
      driveLoco(operand);
      break;
    case OPCODE_RESERVE:
      if (flags[operand] & SECTION_FLAG) {
        DIAG("Wait section ");
        driveLoco(0);
        return;
      }
      DIAG("Reserved section");
      flags[operand] |= SECTION_FLAG;
      break;
    case OPCODE_FREE:
      flags[operand] &= ~SECTION_FLAG;
      break;
    case OPCODE_AT:
      if (readSensor(operand)) break;
      DIAG("\nWAIT %d",operand);
      return;
    case OPCODE_PASSED: // waits for sensor to hit and then remain off for 0.5 seconds.
      { 
        bool hit=readSensor(operand);
      
      if (hit) {
        task->waitingFor=millis()+500;  // reset timer to half a second
        return;
      }
      else {
        if (task->waitingFor == 0 ) return; // not yet reached sensor 
        if (task->waitingFor>millis()) return; // sensor off but not long enough
        task->waitingFor =0; // all done now
        break;
      } 
      }

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
    case OPCODE_ENDPROG:
    case OPCODE_ENDROUTES:
      task->progCounter=-1; // task not in use any more
      DIAG("Task Terminated");
      return;
      case OPCODE_PROGTRACK:
       DIAG("\n progtrack %d pin %d",operand,progTrackPin);
       if (operand>0) {
        // set progtrack on. drop dccpp register
        flags[task->reg] &= ~REGISTER_FLAG;
        task->reg=0;
        digitalWrite(progTrackPin, HIGH);
       }
       else {
            for (int reg=1;reg<16;reg++) {
              if (! (flags[reg] & REGISTER_FLAG)) {
                task->reg=reg;
                flags[reg] |= REGISTER_FLAG;
                break;
              }
            }
            digitalWrite(progTrackPin, LOW);
       }
       break;
      case OPCODE_READ_LOCO:
       if (!readLoco()) return;
       break;
       case OPCODE_ROUTE:
       DIAG("\n Starting Route %d\n",operand);
       break;
       case OPCODE_PAD:
       break;
    default:
      DIAG("Opcode not supported");
    }
    task->progCounter+=2;
    }
