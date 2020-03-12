
#include <DCCpp.h>
class autotask {       // The class
  public:             // Access specifier
     int progCounter;  
    unsigned long waitingFor;
    int loco;
    short reg;
    short locosteps;
    bool forward;
    int speedo;
    autotask* next;
    FunctionsState functions;
    public: autotask();

};
