#include <FaBoLCD_PCF8574.h>
class TPLDisplay : public FaBoLCD_PCF8574 {
  public:
  TPLDisplay(void)
        : FaBoLCD_PCF8574{ 0x3F }
    {
    }
};
