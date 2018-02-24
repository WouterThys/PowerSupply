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
    
#define SPI2_SCK_Dir    TRISBbits.TRISB10       /* SCK pin on RB7             */
#define SPI2_SDO_Dir    TRISBbits.TRISB11       /* SDO pin on RB8             */
#define SPI2_SDI_Dir    TRISBbits.TRISB12       /* SDI pin on RB9             */

#define SPI2_SS_Dir     TRISBbits.TRISB13       /* SS on pin RB10             */
#define SPI2_SS_Pin     PORTBbits.RB13          /* SS pin                     */

#define SPI2_SCK_Map    RPOR4bits.RP42R         /* RP42 Pin Assignment  SCK   */
#define SPI2_SDO_Map    RPOR4bits.RP43R         /* RP43 Pin Assignment  SDO   */
#define SPI2_SDI_Map    0b0101100               /* RPI44 Pin Assignment SDI   */

/**
 * I²C
 */
#define I2C_ADDRESS     0x03

#define I2C_SCL_Dir     TRISBbits.TRISB8        /* SCL Direction              */
#define I2C_SDA_Dir     TRISBbits.TRISB9        /* SDA Direction              */

#define I2C_SCL_Odc     ODCBbits.ODCB8          /* Open drain config for SCL  */
#define I2C_SDA_Odc     ODCBbits.ODCB9          /* Open drain config for SDA  */



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

