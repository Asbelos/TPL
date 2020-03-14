/*!
 *  @file PWMServoDriver.h
 *
 *  Used to set servo positions on an I2C bus with 1 or more PCA96685 boards.
 */
#ifndef PWMServoDriver_H
#define PWMServoDriver_H


class PWMServoDriver {
public:
  
  static void begin(short servoCount = 16);
  static void setServo(short servoNum,  uint16_t pos);
 
private:
  static void setup(int board);
  static void writeRegister(uint8_t i2caddr,uint8_t hardwareRegister, uint8_t value);
};

#endif
