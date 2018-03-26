/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef SETTINGS_H
#define	SETTINGS_H

#include <xc.h> // include processor files - each processor file is guarded.  

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * Interrupt priorities
 */

#define MI2C_IP     1



    
/**
 *  PIC
 */
    
  
    
/**
 * LCD
 */    
#define D0              PORTBbits.RB6
#define D1              PORTBbits.RB7
#define D2              PORTBbits.RB10
#define D3              PORTBbits.RB11
#define D4              PORTBbits.RB12
#define D5              PORTBbits.RB13
#define D6              PORTBbits.RB14
#define D7              PORTBbits.RB15
    
#define DATA_Dir        TRISB
#define DATA_DirMask    0x033F /* Mask for data port directions */
    
#define LCD_E           PORTAbits.RA2
#define LCD_RS          PORTAbits.RA3
#define LCD_RW          PORTAbits.RA4
    

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

