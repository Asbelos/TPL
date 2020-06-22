#include <Arduino.h>
#include <Adafruit_MCP23017.h>
class TPLSensors {
   public:
    static void init(byte maxsensors);
    static bool readSensor(byte pin);
    private:
    static Adafruit_MCP23017 mcp;
};
