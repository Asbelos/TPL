#include <arduino.h>
#include "autotask.h"

  autotask::autotask() {
      waitingFor=0;
      reg=0;
      loco=0;
      locosteps=0;
      progCounter=0;
      forward=true;
      functions.clear();
    }

   
