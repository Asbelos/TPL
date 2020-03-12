/*!
 *  @file PWMServoDriver.cpp
 *
 *  @mainpage Adafruit 16-channel PWM & Servo driver, based on Adafruit_PWMServoDriver
 *
 *  @section intro_sec Introduction
 *
 *  This is a library for the 16-channel PWM & Servo driver.
 *
 *  Designed specifically to work with the Adafruit PWM & Servo driver.
 *  This class contains a very small subset of the Adafruit version which
 *  is relevant to driving simple servos at 50Hz through a number of chained 
 *  servo driver boards (ie servos 0-15 on board 0x40, 16-31 on board 0x41 etc.)
 *  
 *  @section author Author
 *  Chris Harlow (TPL) 
 *  original by Limor Fried/Ladyada (Adafruit Industries).
 *
 *  @section license License
 *
 *  BSD license, all text above must be included in any redistribution
 */

#include "PWMServoDriver.h"
#include <Wire.h>
#define DIAG_ENABLED true
#include "DIAG.h"

// REGISTER ADDRESSES
#define PCA9685_MODE1 0x00      // Mode Register 

#define PCA9685_FIRST_SERVO 0x06  /** low byte first servo register ON*/
#define PCA9685_PRESCALE 0xFE     /**< Prescaler for PWM output frequency */
// MODE1 bits
#define MODE1_SLEEP 0x10   /**< Low power mode. Oscillator off */
#define MODE1_AI 0x20      /**< Auto-Increment enabled */
#define MODE1_RESTART 0x80 /**< Restart enabled */


#define PCA9685_I2C_ADDRESS 0x40      /**< First PCA9685 I2C Slave Address */
#define FREQUENCY_OSCILLATOR 25000000.0 /**< Int. osc. frequency in datasheet */




/*!
 *  @brief  Sets the PWM frequency for a chip to 50Hz for servos
 */

void PWMServoDriver::setup(int board) {
  const uint8_t PRESCALE_50HZ = (uint8_t)(((FREQUENCY_OSCILLATOR / (50.0 * 4096.0)) + 0.5) - 1);
  uint8_t i2caddr=PCA9685_I2C_ADDRESS + board;
  uint8_t mode1 = read8(i2caddr, PCA9685_MODE1); 
  write8(i2caddr,PCA9685_MODE1, (mode1 & ~MODE1_RESTART) | MODE1_SLEEP);     
  write8(i2caddr,PCA9685_PRESCALE, PRESCALE_50HZ);  
  write8(i2caddr,PCA9685_MODE1,mode1);
  delay(5);
  write8(i2caddr,PCA9685_MODE1, mode1 | MODE1_RESTART | MODE1_AI);
}

/*!
 *  @brief  Setups the I2C interface and hardware
 *  @param  servoCount
 *          Sets enough chained boards for the attached Servos
 */
void PWMServoDriver::begin(short servoCount) {
    Wire.begin();
    int boards=(servoCount+15)/16;  // 16 servos per board     
    for (int board=0;board<boards;board++) { 
        setup(board);
    }
}
/*!
 *  @brief  Sets the PWM output to a servo
 */
void PWMServoDriver::setServo(short servoNum, uint16_t value) {
  DIAG("\nsetServo %d %d\n",servoNum,value);
  // This works because MODE1_AI auto increments the register number after each byte
  Wire.beginTransmission(PCA9685_I2C_ADDRESS + servoNum/16);
  Wire.write(PCA9685_FIRST_SERVO + 4 * (servoNum % 16) );
  Wire.write(0);
  Wire.write(0);
  Wire.write(value);
  Wire.write(value >> 8);
  Wire.endTransmission();
}



/******************* Low level I2C interface */
uint8_t PWMServoDriver::read8(uint8_t i2caddr,uint8_t addr) {
  Wire.beginTransmission(i2caddr);
  Wire.write(addr);
  Wire.endTransmission();
  Wire.requestFrom(i2caddr,(uint8_t)1);
  return Wire.read();
}

void PWMServoDriver::write8(uint8_t i2caddr,uint8_t addr, uint8_t d) {
  Wire.beginTransmission(i2caddr);
  Wire.write(addr);
  Wire.write(d);
  Wire.endTransmission();
}
