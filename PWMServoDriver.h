/*!
 *  @file Adafruit_PWMServoDriver.h
 *
 *  This is a library for our Adafruit 16-channel PWM & Servo driver.
 *
 *  Designed specifically to work with the Adafruit 16-channel PWM & Servo
 * driver.
 *
 *  Pick one up today in the adafruit shop!
 *  ------> https://www.adafruit.com/product/815
 *
 *  These driver use I2C to communicate, 2 pins are required to interface.
 *  For Arduino UNOs, thats SCL -> Analog 5, SDA -> Analog 4.
 *
 *  Adafruit invests time and resources providing this open source code,
 *  please support Adafruit andopen-source hardware by purchasing products
 *  from Adafruit!
 *
 *  Limor Fried/Ladyada (Adafruit Industries).
 *
 *  BSD license, all text above must be included in any redistribution
 */
#ifndef PWMServoDriver_H
#define PWMServoDriver_H

#include <Arduino.h>
#include <Wire.h>

/*!
 *  @brief  Class that stores functions for interacting with PCA9685
 * PWM chip
 */
class PWMServoDriver {
public:
  
  static void begin(short servoCount = 16);
  static void setServo(short servoNum,  uint16_t pos);
 
private:
  static void setup(int board);
  static uint8_t read8(uint8_t i2caddr,uint8_t addr);
  static void write8(uint8_t i2caddr,uint8_t addr, uint8_t d);
};

#endif
