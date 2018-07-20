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

#define DEBUG       1           /* General debug enable                       */
#define DEBUG_I2C  (1 & DEBUG)  /* I²C debug enable                           */
#define DEBUG_FSM  (1 & DEBUG)  /* FSM debug enable                           */

/**
 * Interrupt priorities (Highest (7) - Lowest(1))
 */

#define IP_U1RX      6
#define IP_I2C       5
#define IP_MENU_TMR  2
#define IP_MAIN_TMR  1
    
/**
 *  PIC
 */
 
#define LED1            PORTAbits.RA0
#define LED1_Dir        TRISAbits.TRISA0
#define LED2            PORTAbits.RA1
#define LED2_Dir        TRISAbits.TRISA1
#define LED3            PORTBbits.RB0
#define LED3_Dir        TRISBbits.TRISB0
    
/**
 * UART
 */      
#define UART1_BAUD      57600 /* 57600 */
#define UART1_ID        2   

#define UART1_RX_Dir    TRISBbits.TRISB4    /* RX Pin on RP36-RB4             */
#define UART1_TX_Dir    TRISAbits.TRISA4    /* TX Pin on RP20-RA4             */

#define UART1_RX_Map    0b0100100           /* RX on RP36                     */
#define UART1_TX_Map    RPOR0bits.RP20R     /* TX on RP20                     */


/**
 * MCP
 */
#define MCP_CS          PORTBbits.RB1
#define MCP_CS_Dir      TRISBbits.TRISB1
#define MCP_ADDR        0b01000000

#define MCP_INTA        PORTBbits.RB14
#define MCP_INTA_Dir    TRISBbits.TRISB14
#define MCP_INTA_Pu     CNPUBbits.CNPUB14

#define MCP_INTB        PORTBbits.RB15
#define MCP_INTB_Dir    TRISBbits.TRISB15
#define MCP_INTB_Pu     CNPUBbits.CNPUB15

/**
 * LCD
 */
#define LCD_SS_Dir      TRISBbits.TRISB13       /* SS on pin RB10             */
#define LCD_SS_Pin      PORTBbits.RB13          /* SS pin                     */

/**
 *  SPI1
 */
    
#define SPI1_SCK_Dir    TRISBbits.TRISB7        /* SCK pin on RB7             */
#define SPI1_SDO_Dir    TRISBbits.TRISB8        /* SDO pin on RB8             */
#define SPI1_SDI_Dir    TRISBbits.TRISB9        /* SDI pin on RB9             */

/**
 *  SPI2
 */
    
#define SPI2_SCK_Dir    TRISBbits.TRISB10       /* SCK pin on RB7             */
#define SPI2_SDO_Dir    TRISBbits.TRISB11       /* SDO pin on RB8             */
#define SPI2_SDI_Dir    TRISBbits.TRISB12       /* SDI pin on RB9             */

#define SPI2_SCK_Map    RPOR4bits.RP42R         /* RP42 Pin Assignment  SCK   */
#define SPI2_SDO_Map    RPOR4bits.RP43R         /* RP43 Pin Assignment  SDO   */
#define SPI2_SDI_Map    0b0101100               /* RPI44 Pin Assignment SDI   */

/**
 * I²C
 */
#define I2C_MASTER      /* Or I2C_SLAVE                                       */
#define I2C_WORD_WIDE   /* All data is send as 16-bit                         */
#define I2C_ADDRESS     0x55   

#define I2C_SCL_Dir     TRISBbits.TRISB6        /* SCL Direction              */
#define I2C_SDA_Dir     TRISBbits.TRISB5        /* SDA Direction              */

#define I2C_SCL_Odc     ODCBbits.ODCB6          /* Open drain config for SCL  */
#define I2C_SDA_Odc     ODCBbits.ODCB5          /* Open drain config for SDA  */



/**
 * ENCODER
 */

    

#endif	/* XC_HEADER_TEMPLATE_H */

