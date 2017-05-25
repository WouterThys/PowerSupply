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

#define CN_IP       2     
#define T2_IP       1  
    
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
#define ENC_Pin_1       PORTBbits.RB10      /* First turn pin of rotary       */
#define ENC_Pin_2       PORTBbits.RB11      /* Sencond turn pin of rotary     */
#define ENC_Pin_3       PORTBbits.RB12      /* Push button pin of rotary      */
#define ENC_Dir_1       TRISBbits.TRISB10
#define ENC_Dir_2       TRISBbits.TRISB11
#define ENC_Dir_3       TRISBbits.TRISB12

/**
 * Clock
 */
#define CLK_Pin         PORTBbits.RB7
#define CLK_Dir         TRISBbits.TRISB7

    

#endif	/* XC_HEADER_TEMPLATE_H */

