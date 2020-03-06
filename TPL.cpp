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
short flags[64];
const short **tplRoutes;

short sensorZeroPin;
short sensorMax;

void tplSensorZeroPin(short _sensorZeroPin, short _sensors) {
  sensorZeroPin = _sensorZeroPin;
  sensorMax=_sensors-1;
  for (int pin = 0; pin < _sensors; pin++) {
    pinMode(pin + sensorZeroPin, INPUT_PULLUP);
  }
}

short signalZeroPin;
void tplSignalsZeroPin(short _signalZeroPin, short _signals) {
  signalZeroPin = _signalZeroPin;
  for (int pin = 0; pin < _signals + _signals ; pin++) {
    pinMode(pin + signalZeroPin, OUTPUT);
  }
}



void tplAddRoutes(const short * _routes[]) {
  tplRoutes = _routes;
}

void tplAddTask(const short* _route) {
  tplAddJourney(_route, 0, 0, 0);
}

void tplAddJourney(const short* _route, short _reg, short _loco, short _steps) {
  DIAG("\nAddTask Loco=%d\n", _loco);
  autotask* newtask = new autotask();
  newtask->loco = _loco;
  newtask->locosteps = _steps;
  newtask->reg = _reg;
  newtask->route = _route;
  
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
    short opcode = task->nextOp();
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

void tplLoop() {
  if (task == NULL ) return;
   task = task->next;
  if (task->progCounter < 0) return;
  short opcode = task->nextOp();
  short operand = task->nextOperand();
  // DIAG("\nSTEP Loco=%d prog=%d opcode=%d operand=%d ", task->loco, task->progCounter, opcode, operand);
  switch (opcode) {
    case OPCODE_TL:
    case OPCODE_TR:
      if (delayme(1)) return;
      if (!TPLTurnout::slowSwitch(operand, opcode==OPCODE_TL)) return;
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
    case OPCODE_NOTAT:
      if (!readSensor(operand)) break;
      DIAG("\nWAIT off %d",operand);
      return;
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
    case OPCODE_PAD:
      break;
    case OPCODE_STOP:
      driveLoco(0);
      break;
    case OPCODE_AGAIN:
      task->progCounter=0;
      return;
    case OPCODE_FOLLOW:
      task->progCounter=0;
      task->route=tplRoutes[operand];
      DIAG("Switched route to % d",opcode);
      return;
    case OPCODE_ENDPROG:
      task->progCounter=-1; // task not in use any more
      DIAG("Task Terminated");
      return;
    default:
      DIAG("Opcode not supported");
    }
    task->progCounter+=2;
    }
