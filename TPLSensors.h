class TPLSensors {
   public:
    static void init(byte maxsensors);
    static bool readSensor(byte pin);
   private:
    static void  writeRegister(byte deviceAddr,byte reg, byte value);
    static byte  readRegister(byte deviceAddr,byte reg);
};
