#include <arduino.h>
#include "autotask.h"

  autotask::autotask() {
      waitingFor=0;
      reg=0;
      loco=0;
      locosteps=0;
      progCounter=0;
      forward=true;
    }

   short autotask::nextOp() {
      return pgm_read_byte_near(route+progCounter);
    }
    
    short autotask::nextOperand() {      
      return pgm_read_byte_near(route+progCounter+1);
    }
