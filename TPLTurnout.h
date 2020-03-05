#include <Turnout.h>
class tplTurnout: public Turnout {
  public :
  static void setup(short _turnoutBoard0, short _turnouts);
  static tplTurnout* create(int id, int add, int subAdd);
  void activate(int s) override ;   
  static void activate(short id, bool left);
  };
