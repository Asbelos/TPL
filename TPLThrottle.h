class TPLThrottle {
  public:
   static void begin();
   static void zero(); 
   static int count();
   static char getKey();
   static const int QUIT_MANUAL=999;
   private:
   static void isrA();
   static volatile int counter;
   static unsigned char state;
  
};
