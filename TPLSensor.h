#ifndef TPLSensor_h
#define TPLSensor_h
class TPLSensor {
  public:
    TPLSensor(byte id, bool isI2C, byte pin);
    static bool read(byte id);
  private:
    static Adafruit_MCP23017 * mcp[4];
    static TPLSensor* firstSensor;
    bool read();
    TPLSensor* nextSensor;
    byte id;
    bool isI2C;
    byte pin;
};

#endif
