/** 
 *  Ideas list:
 *    track signals state in flags array
 *    IFRED, IFGREEN, WAITRED, WAITGREEN operators
 *    LCD status board
 *    
 *    Implement manual controller in HW and SW as separate class.
 *    Change TPL to class
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
 *     Replace sensor inputs to shift registers. 
 *     Replace signals  to handle via PCA9685 
 *  
 *  
 *  
 *  
 *  
 */
