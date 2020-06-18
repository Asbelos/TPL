class TPLI2C {
 public:
   static void begin();
   static void write(uint8_t i2caddr,byte buffer[], byte size);
   static void  writeRegister(byte deviceAddr,byte reg, byte value);
   static uint16_t  read2Registers(byte deviceAddr,byte reg);
};
