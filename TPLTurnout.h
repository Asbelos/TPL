#include "Turnouts.h"

class TPLTurnout : Turnout {
  public :
  static void begin(int maxTurnouts);
  void activate(bool left) override;
  };
