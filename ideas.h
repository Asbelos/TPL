/** 
 *  Ideas list:
 *    track signals state in flags array
 *    IFRED, IFGREEN, WAITRED, WAITGREEN operators
 *    LCD status board
 *    
 *    Implement manual controller in HW and SW as separate class.
 
 *    
 *        
 *        Sensor Notes:
 *        sensors are numbered 0-63
 *        sensors below sensorMax (see tplSetSensorPins  are mapped to hardware pins, the others are logical only
 *        Hardware sensors may be latched ON by software, but when unlatched will revert to reading the pins. 
 *  
 *     Turnout Notes:
 *       1) addred virtual keyword to Turnouts.h activate() function to allow override from TPL
 *     
 *     
 *     Replace signals  to handle via PCA9685 

 */
 //  // a route drives an engine over some part of the track
// ROUTE(1) // 1-8-3-7-2
//  AT(0)  
//  DELAYRANDOM(50,200)  RESERVE(8)  RESERVE(5)   TL(1)  GREEN(1)  RESET(9)  RESET(5)  FWD(25)
//  AT(9)  RED(1) 
//  AT(5)  FREE(1)  RESERVE(9)  TL(3)  TL(4)  GREEN(5)  RESET(8)  FWD(20)  
//  AT(8)  STOP  RED(5)  FREE(5)  FREE(9)  
//  DELAYRANDOM(100,200)  RESERVE(3)  RESERVE(6)  RESERVE(9)  TL(4)  TR(3)  TL(2)  GREEN(8)  RESET(6)  RESET(3)  REV(35)
//  AT(6)  FREE(3)  RED(8)  FREE(8)  FREE(9)  
//  AT(3)  STOP  FREE(6)
//  DELAYRANDOM(100,200)  RESERVE(7)  RESERVE(6)  TL(2)  GREEN(3)  RESET(10)  RESET(6)  FWD(25)
//  AT(10)  RED(3)  
//  AT(6)  RESERVE(9)  TR(3)  TR(4)  GREEN(6)  FWD(25)
//  AT(7)  STOP  RED(6)  FREE(6)  FREE(9)  
//  DELAYRANDOM(100,200)  RESERVE(2)  RESERVE(5)  TL(1)  RESERVE(9)  TR(4)  TL(3)  GREEN(7)  RESET(5)  RESET(2)  REV(5)
//  AT(5)  RED(7)  FREE(7)  FREE(9)  TR(1)  
//  AT(2)  STOP  FREE(5)
//  FOLLOW(2)
// 
// ROUTE(2) // 2-8-4-7-1
//  AT(0)
//  DELAYRANDOM(50,200)
//  RESERVE(8)
//  RESERVE(5) 
//  TR(1)
//  GREEN(2)
//  RESET(9)
//  RESET(5)
//  FWD(5)
//  AT(9)
//  RED(2)
//  AT(5)
//  STOP
//  FREE(2)
//  RESERVE(9)
//  TL(3)
//  TL(4)
//  GREEN(5)
//  RESET(8)
//  FWD(5)
//  AT(8)
//  STOP
//  RED(5)
//  FREE(5)
//  FREE(9)
//  DELAYRANDOM(100,200)
//  RESERVE(4)
//  RESERVE(6)
//  RESERVE(9)
//  TL(4)
//  TR(3)
//  TR(2)
//  GREEN(8)
//  RESET(6)
//  RESET(4)
//  REV(5)
//  AT(6)
//  RED(8)
//  FREE(8)
//  FREE(9)
//  AT(4)
//  STOP
//  FREE(6)
//  DELAYRANDOM(100,200)
//  RESERVE(7)
//  RESERVE(6)
//  TR(2)
//  GREEN(4)
//  RESET(10)
//  RESET(6)
//  FWD(5)
//  AT(10)
//  FREE(4)
//  RED(4)
//  AT(6)
//  RESERVE(9)
//  TR(3)
//  TR(4)
//  GREEN(6)
//  RESET(7)
//  FWD(5)
//  AT(7)
//  STOP
//  RED(6)
//  FREE(6)
//  FREE(9)
//  DELAYRANDOM(100,200)
//  RESERVE(1)
//  RESERVE(5)
//  TR(1)
//  RESERVE(9)
//  TR(4)
//  TL(3)
//  GREEN(7)
//  RESET(5)
//  RESET(1)
//  REV(5)
//  AT(5)
//  TL(1)
//  RED(7)
//  FREE(7)
//  FREE(9)
//  AT(1)
//  STOP
//  FREE(5)
//  FOLLOW(1)
// 
//  
//  
