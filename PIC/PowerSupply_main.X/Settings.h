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

/**
 * Interrupt priorities
 */

#define T2_IP     1     /* Timer 1 priority for Rotary encoder                */
    
/**
 *  PIC
 */
 
#define LED1            PORTBbits.RB15
#define LED1_Dir        TRISBbits.TRISB15
#define LED2            PORTBbits.RB14
#define LED2_Dir        TRISBbits.TRISB14
    
/**
 * ENCODER
 */ 
#define ENC_Pin_1       PORTBbits.RB10
#define ENC_Pin_2       PORTBbits.RB11
#define ENC_Dir_1       TRISBbits.TRISB10
#define ENC_Dir_2       TRISBbits.TRISB11
    

#endif	/* XC_HEADER_TEMPLATE_H */

