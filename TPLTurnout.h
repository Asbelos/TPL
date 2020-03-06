#include <Turnout.h>

class TPLTurnout: public Turnout {
  public :
  static void SetTurnouts(short _turnoutBoard0, short _turnouts);
  static TPLTurnout* create(int id, int add, int subAdd);
  void activate(int s) override ;   
  static bool slowSwitch(short id, bool left);
  int currentPos;
  };
