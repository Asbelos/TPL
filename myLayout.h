   // Describe the turnouts (use  a loop if it helps)
   //       TPL refers to turnouts by id and switches them left or right to avoid ambiguity.
    
   //   TPL::I2CTURNOUT(byte id, byte pin, int servoLeft, int servoRight);
   //   TPL::DCCTURNOUT(byte id, int dccAddress, byte subAddress, bool activatedIsLeft);
 
   // I2C turnouts TPL::I2CTURNOUT(byte id, byte pin, int servoLeft, int servoRight)
   //               id  = unique id (0-64)  used in TL() and TR() and JMRI (if used)
   //               pin   = pin position relative to first PWM servo board (second board starts at 16 etc)
   //               leftAngle  = servo position for turn left    
   //               rightAngle = servo position for turn right
   //   
   // DCC turnouts TPL::DCCTURNOUT(byte id, int dccAddress, byte subAddress, bool activatedIsLeft)
   //               id = unique id (0-64) used in TL() and TR() and JMRI (if used)
   //               dccAddress = DCC accessory decoder address
   //               subAddress  = accesory sub address    
   //               activatedIsLeft = true if point thrown means left exit.
 

   TPL::I2CTURNOUT(0, 0, 150, 195);       
   TPL::I2CTURNOUT(1, 1, 150, 195);       
   TPL::I2CTURNOUT(2, 2, 150, 195);       
   TPL::I2CTURNOUT(3, 3, 150, 195);       
   TPL::I2CTURNOUT(4, 4, 150, 195);       
   TPL::I2CTURNOUT(5, 5, 150, 195);       
   TPL::I2CTURNOUT(6, 8, 150, 195);       
   TPL::I2CTURNOUT(7, 9, 150, 195);       
   TPL::I2CTURNOUT(8,11, 150, 195);       

   // Describe the sensors
   // I2C Sensors
   // Pin Sensors

   // Describe Signals 
   // I2C Signals
   // Pin Signals 
   // DCC Signals 

   // Other layout specific setup code may appear here.  
