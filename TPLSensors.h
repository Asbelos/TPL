class TPLSensors {
   public:
    static void init(byte maxsensors);
    static bool readSensor(byte pin);
};
