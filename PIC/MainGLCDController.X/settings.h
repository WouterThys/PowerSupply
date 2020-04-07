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
#include <math.h>

    
/**
 *  PIC
 */
#define _XTAL_FREQ 48000000
 
#define LED1            PORTAbits.RA0
#define LED1_Dir        TRISAbits.TRISA0
#define LED2            PORTAbits.RA1
#define LED2_Dir        TRISAbits.TRISA1
#define LED3            PORTCbits.RC4
#define LED3_Dir        TRISCbits.TRISC4
    

/**
 *  GLCD
 */
#define GLCD_DOUT_REG    PORTD
#define GLCD_DIN_REG     PORTD
#define GLCD_DDIR_REG    TRISD

#define CSEL1       PORTEbits.RE0
#define CSEL2       PORTEbits.RE1
#define CSEL3       PORTEbits.RE2
#define D_I         PORTBbits.RB4
#define R_W         PORTBbits.RB3
#define RST         PORTBbits.RB5
#define EN          PORTBbits.RB2

#define CS1_Dir TRISEbits.TRISE0
#define CS2_Dir TRISEbits.TRISE1
#define CS3_Dir TRISEbits.TRISE2
#define RS_Dir  TRISBbits.TRISB4
#define RW_Dir  TRISBbits.TRISB3
#define RST_Dir TRISBbits.TRISB5
#define EN_Dir  TRISBbits.TRISB2


/**
 * UART
 */
#define UART_TX_Pin PORTCbits.RC6
#define UART_RX_Pin PORTCbits.RC7
#define UART_TX_Dir TRISCbits.RC6
#define UART_RX_Dir TRISCbits.RC7

#define UART_BAUD   9600
#define UART_INVERT 0

#endif	/* XC_HEADER_TEMPLATE_H */

