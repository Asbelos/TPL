

class autotask {       // The class
  public:             // Access specifier
    const short* route;       
    int progCounter;  
    unsigned long waitingFor;
    int loco;
    short reg;
    short locosteps;
    bool forward;
    int speedo;
    autotask* next;

    public: autotask();

    public: short nextOp();
    
    public: short nextOperand();
};
