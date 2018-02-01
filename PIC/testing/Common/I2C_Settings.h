/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef I2C_SETTINGS_H
#define	I2C_SETTINGS_H

#include <xc.h> // include processor files - each processor file is guarded.  

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */  
    
/**
 * I2C
 */   
#define I2C_UART_ADDRESS     0x55
#define I2C_VARIABLE_ADDRESS 0x56
#define I2C_LCD_ADDRESS      0x57    

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

