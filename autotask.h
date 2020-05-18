

class autotask {       // The class
  public:             // Access specifier
    int progCounter;    // Byte offset of next route opcode in ROUTES table
    unsigned long waitingFor; // Used by opcodes that must be recalled before completing
    unsigned long waitAfter; // Used by OPCODE_AFTER
    int loco;
    bool forward;
    bool invert;
    int speedo;
    autotask* next;
    public: autotask();

};
