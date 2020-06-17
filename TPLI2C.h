class TPLI2C {
   public:
   
   static void begin();
   static void beginTransmission(uint8_t i2caddr);
   static void write(uint8_t value);
   static void write(uint8_t i2caddr,byte buffer[], byte size);

   static void endTransmission();

    static void  writeRegister(byte deviceAddr,byte reg, byte value);
    static byte  readRegister(byte deviceAddr,byte reg);
};
