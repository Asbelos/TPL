#include <arduino.h>
#include "autotask.h"

  autotask::autotask() {
      waitingFor=0;
      loco=0;
      progCounter=0;
      forward=true;
      invert=false;
    }

   
