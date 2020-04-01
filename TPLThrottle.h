class TPLThrottle {
  public:
   static void begin();
   static void zero();
   static void loop();
   static int count();
   private:
   static int counter;
   static byte aLastState;
};
