/** 
 *  Ideas list:
 *    track signals state in flags array
 *    IFRED, IFGREEN, WAITRED, WAITGREEN operators
 *    Wait for sensor off operator
 *    LCD status board
 *    
 *    Implement manual controller in HW and SW as separate class.
 *    Change TPL to class
 *    Implement launch control (
 *        progtract to prog, read locoid and steps, check direction.
 *        progtract to main, create task for launch route.
 *        Launch route from progtract to station 1.
 *        )
 *        
 *        Implement unload control... similar to launch in reverse
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
