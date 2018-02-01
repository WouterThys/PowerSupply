/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef COM_SETTINGS_H
#define	COM_SETTINGS_H

/**
 * Standard command:
 * * * * * * * * * *
 * 
 *  CM0 CM1 CM2 | ST0 ST1 ST2 | DA0 DA1
 * 
 *  - CM<0:2> : Command
 *  - ST<0:2> : FSM state of main  
 *  - DA<0:1> : Extra data bits
 * 
 * 
 * LCD command: (Needs to be proceeded by a standard command)
 * * * * * * * * * *
 * 
 *  CM0 CM1 | DA0 DA1 DA2 DA3 DA4 DA5 DA6
 * 
 *  - CM<0:1> : LCD command, depending on command DA<0:6> have different meaning
 * 
 *  * DRAW =    0 1 | ME0 ME1 ME2 | SM0 SM1 SM2  
 *    - ME<0:1> : ID of menu to draw
 *    - SM<0:2> : ID of sub menu to draw    
 *  
 *  * SELECT =  1 0 | W | S | ID0 ID1 ID2 ID3
 *    - W : What - TYPE_ARROW or TYPE_FIELD
 *    - S : Select - Select or not (Blink on LCD)
 *    - ID<0:3> : ID of item to select
 * 
 *  * SET =     1 1 | <FIELD ID>
 *    - Command has field id to set, the I2C master should have written the
 *      value to its i2cData_t data1 and data2. 
 */

// Common commands
#define COM_INIT             0x00
#define COM_ENABLE           0x01
#define COM_READ             0x02
#define COM_ASK              0x03
#define COM_SET              0x04
#define COM_LCD              0x05
#define COM_STATUS           0x06

// Commands for variable controller
#define COM_VAR_READ         0x01
#define COM_VAR_WRITE        0x02

// Commands for LCD controller
#define COM_LCD_START        0x0A
#define COM_LCD_STOP         0x0B

#define COM_LCD_COM_NONE     0x00
#define COM_LCD_COM_DRAW     0x01
#define COM_LCD_COM_SELECT   0x02
#define COM_LCD_COM_SET      0x03

// Commands for ...



/**
 * Status stuff
 * * * * * * * *
 * 
 * Status is 4 bits and part of the answer of D_I2C_MasterRead()
 * 
 *  B0 B1 E I
 *   - I : Initialized
 *   - E : Enabled 
 */

#define STA_INITIALIZED     0x01
#define STA_ENABLED         0x02

#define STA_INITIALIZED_MSK 0x01
#define STA_ENABLED_MSK     0x02


/**
 * Result stuff
 * * * * * * * *
 * 
 * Almost all commandos have a result.
 * 
 * I2C results can also be returned as is this result codes do not overlap
 * with the I2C result codes
 */

#define RES_ENA_OK      3
#define RES_INIT_OK     2
#define RES_OK          1
#define RES_NOK         -10
#define RES_INIT_NOK    -11
#define RES_ENA_NOK     -12


#endif	/* XC_HEADER_TEMPLATE_H */

