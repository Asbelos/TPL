class TPLI2C {
   public:
    static void  writeRegister(byte deviceAddr,byte reg, byte value);
    static byte  readRegister(byte deviceAddr,byte reg);
};
