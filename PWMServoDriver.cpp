

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
#include <Arduino.h>
#include "PWMServoDriver.h"
#include "TPLI2C.h"
#define DIAG_ENABLED true
#include "DIAG.h"

// REGISTER ADDRESSES
#define PCA9685_MODE1 0x00      // Mode Register 
#define PCA9685_FIRST_SERVO 0x06  /** low byte first servo register ON*/
#define PCA9685_PRESCALE 0xFE     /** Prescale register for PWM output frequency */
// MODE1 bits
#define MODE1_SLEEP 0x10   /**< Low power mode. Oscillator off */
#define MODE1_AI 0x20      /**< Auto-Increment enabled */
#define MODE1_RESTART 0x80 /**< Restart enabled */

#define PCA9685_I2C_ADDRESS 0x40      /** First PCA9685 I2C Slave Address */
#define FREQUENCY_OSCILLATOR 25000000.0 /** Accurate enough for our purposes  */

/*!
 *  @brief  Sets the PWM frequency for a chip to 50Hz for servos
 */

void PWMServoDriver::setup(int board) {
  const uint8_t PRESCALE_50HZ = (uint8_t)(((FREQUENCY_OSCILLATOR / (50.0 * 4096.0)) + 0.5) - 1);
  uint8_t i2caddr=PCA9685_I2C_ADDRESS + board;
  DIAG(F("\nPWMServoDrive::setup(%d) prescale=%d"),board,PRESCALE_50HZ); 
  TPLI2C::writeRegister(i2caddr,PCA9685_MODE1, MODE1_SLEEP | MODE1_AI); 
  delay(5);    
  TPLI2C::writeRegister(i2caddr,PCA9685_PRESCALE, PRESCALE_50HZ);  
  delay(5);
  TPLI2C::writeRegister(i2caddr,PCA9685_MODE1,MODE1_AI);
  delay(5);
  TPLI2C::writeRegister(i2caddr,PCA9685_MODE1,  MODE1_RESTART | MODE1_AI);
  delay(5);
}

/*!
 *  @brief  Setups the I2C interface and hardware
 *  @param  servoCount
 *          Sets enough chained boards for the attached Servos
 */
void PWMServoDriver::begin(short servoCount) {
    TPLI2C::begin();
    int boards=(servoCount+15)/16;  // 16 servos per board     
    for (int board=0;board<boards;board++) { 
        setup(board);
    }
}
/*!
 *  @brief  Sets the PWM output to a servo
 */
void PWMServoDriver::setServo(short servoNum, uint16_t value) {
  DIAG(F("\nsetServo %d %d\n"),servoNum,value);
  // This works because MODE1_AI auto increments the register number after each byte
  TPLI2C::beginTransmission(PCA9685_I2C_ADDRESS + servoNum/16);
  TPLI2C::write(PCA9685_FIRST_SERVO + 4 * (servoNum % 16) );
  TPLI2C::write(0);
  TPLI2C::write(0);
  TPLI2C::write(value);
  TPLI2C::write(value >> 8);
  TPLI2C::endTransmission();
}
