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


#define DEBUG       1


/**
 * Interrupt priorities
 */

#define CN_IP       2     
#define T2_IP       1  
#define U1RX_IP     3
    
/**
 *  PIC
 */
 
#define LED1            PORTBbits.RB15
#define LED1_Dir        TRISBbits.TRISB15
#define LED2            PORTBbits.RB14
#define LED2_Dir        TRISBbits.TRISB14
    
/**
 * UART
 */      
#define UART1_BAUD      57600
#define UART1_ID        2   

#define UART1_RX_Dir    TRISBbits.TRISB6    /* RX Pin on RP38-RB6             */
#define UART1_TX_Dir    TRISBbits.TRISB5    /* TX Pin on RP37-RB5             */

#define UART1_RX_Map    0b0100110           /* I/O RP38                       */
#define UART1_TX_Map    0b000001


/**
 *  SPI
 */
    
#define SPI1_SCK_Dir    TRISBbits.TRISB7        /* SCK pin on RB7             */
#define SPI1_SDO_Dir    TRISBbits.TRISB8        /* SDO pin on RB8             */
#define SPI1_SDI_Dir    TRISBbits.TRISB9        /* SDI pin on RB9             */

#define SPI1_SS_Dir     TRISBbits.TRISB10       /* SS on pin RB10             */
#define SPI1_SS_Pin     PORTBbits.RB10          /* SS pin                     */


/**
 * ENCODER
 */
#define ENC_A_Dir       TRISBbits.TRISB0
#define ENC_B_Dir       TRISBbits.TRISB1
#define ENC_A_Pin       PORTBbits.RB0
#define ENC_B_Pin       PORTBbits.RB1

#define ENC_BTN_Dir     TRISAbits.TRISA0
#define ENC_BTN_Pin     PORTAbits.RA0
    

#endif	/* XC_HEADER_TEMPLATE_H */

