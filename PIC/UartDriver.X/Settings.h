/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  




#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * Interrupt priorities
 */

#define INT1_IP     6
#define U1RX_IP     5
#define TMR2_IP     4
#define ADC1_IP     2
#define TMR4_IP     1

/**
 * Ports
 */    

// PIC
#define LED1            PORTBbits.RB15
#define LED1_Dir        TRISBbits.TRISB15
#define LED2            PORTBbits.RB14
#define LED2_Dir        TRISBbits.TRISB14
    
// UART
#define UART1_BAUD      57600
#define UART1_ID        2    
    
#define UART1_RX_Dir    TRISBbits.TRISB5
#define UART1_TX_Dir    TRISBbits.TRISB6    
    
#define UART1_RX_Map    0b0100101
#define UART1_TX_Map    0b000001
    
// SPI
#define SPI2_SCK_Dir    TRISBbits.TRISB10
#define SPI2_SDO_Dir    TRISBbits.TRISB11
#define SPI2_FS_Dir     TRISBbits.TRISB9
    
#define SPI2_SCK_Map    0b001001    /* RPnR<5:0> mapping for SCK2 */
#define SPI2_SDO_Map    0b001000    /* RPnR<5:0> mapping for SDO2 */
#define SPI2_FS_Map     0b001010    /* RPnR<5:0> mapping for FS (SS) */
    
// ADC
#define ADC_CHANNELS    4      
    
#define ADC_CURRENT     0
#define ADC_TEMPERATURE 1    
    
#define ADC0_Pin        PORTAbits.RA0
#define ADC0_Dir        TRISAbits.TRISA0
#define ADC0_PNr        1 /* AN0 */
  
#define ADC1_Pin        PORTAbits.RA1
#define ADC1_Dir        TRISAbits.TRISA1
#define ADC1_PNr        2 /* AN1 */

#define ADC2_Pin        PORTBbits.RB0
#define ADC2_Dir        TRISBbits.TRISB0
#define ADC2_PNr        3 /* AN2 */

#define ADC3_Pin        PORTBbits.RB1
#define ADC3_Dir        TRISBbits.TRISB1
#define ADC3_PNr        4 /* AN3 */




#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

