#include <Turnout.h>
class TPLTurnout: public Turnout {
  public :
  static void SetTurnouts( short _turnouts);
  static TPLTurnout* create(int id);
  void activate(int s) override ;   
  static bool slowSwitch(short id, bool left, bool expedite);
  private:
  int currentPos;
  };
