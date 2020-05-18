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

#define DEBUG       0           /* General debug enable                       */
#define DEBUG_I2C  (0 & DEBUG)  /* I²C debug enable                           */
#define DEBUG_FSM  (0 & DEBUG)  /* FSM debug enable                           */
#define DEBUG_LCD  (1 & DEBUG)  /* LCD debug enable                           */


/**
 * Interrupt priorities (Highest (7) - Lowest(1))
 */

#define IP_U1RX      6
#define IP_I2C       5
#define IP_CN        3
#define IP_MENU_TMR  2
#define IP_TMR_4     1
    
/**
 * FSM TIMER CONFIG
 */
#define T4_PERIOD    200 /* In milliseconds (should be between 14us and 900ms)*/

/******************************************************************************/
/*                  PORT MAPPINGS                                             */
/******************************************************************************/  
/*  RA0 /AN0    -> LED1    
 *  RA1 /AN1    -> LED2
 *  RA2         -> ROT3-A 
 *  RA3         -> ROT3-B 
 *  RA4 /RP20   -> ROT3-D
 * 
 *  RB0 /RPI32  -> MCP_INTB
 *  RB1 /RPI33  -> MCP_INTA
 *  RB2 /RPI34  ->
 *  RB3 /RP35   ->
 *  RB4 /RP36   -> MCP_CS
 *  RB5 /RP37   -> I2C-SDA
 *  RB6 /RP38   -> I2C-SCL
 *  RB7 /RP39   -> SCK
 *  RB8 /RP40   -> SDO  
 *  RB9 /RP41   -> SDI
 *  RB10/RP42   -> ROT2-D
 *  RB11/RP43   -> ROT2-B
 *  RB12/RPI44  -> ROT2-A
 *  RB13/RPI45  -> ROT1-D
 *  RB14/RPI46  -> ROT1-B
 *  RB15/RPI47  -> ROT1-A
 * 
 */


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
// Orange = RX
// Geel = TX
#define UART1_BAUD      19200 /* 57600 */
#define UART1_ID        2   

#define UART1_RX_Dir    TRISBbits.TRISB9    /* RX Pin on RP41-RB9            */
#define UART1_TX_Dir    TRISBbits.TRISB8    /* TX Pin on RP40-RB8            */

#define UART1_RX_Map    0b0101001           /* RX on RP41                    */
#define UART1_TX_Map    RPOR3bits.RP40R     /* TX on RP40                    */



/**
 * I²C
 */
#define I2C_MASTER      /* Or I2C_SLAVE                                       */
#define I2C_WORD_WIDE   /* All data is send as 16-bit                         */
#define I2C2
#define I2C_ADDRESS     0x55   

#define I2C_SCL_Dir     TRISBbits.TRISB6        /* SCL Direction              */
#define I2C_SDA_Dir     TRISBbits.TRISB5        /* SDA Direction              */

#define I2C_SCL_Odc     ODCBbits.ODCB6          /* Open drain config for SCL  */
#define I2C_SDA_Odc     ODCBbits.ODCB5          /* Open drain config for SDA  */
 

/**
 * ROTARY ENCODERS
 */
#define ROT1_A_Pin      PORTBbits.RB15
#define ROT1_A_Dir      TRISBbits.TRISB15
#define ROT1_A_Int      CNENBbits.CNIEB15 
#define ROT1_B_Pin      PORTBbits.RB14
#define ROT1_B_Dir      TRISBbits.TRISB14
#define ROT1_B_Int      CNENBbits.CNIEB14 
#define ROT1_D_Pin      PORTBbits.RB13
#define ROT1_D_Dir      TRISBbits.TRISB13
#define ROT1_D_Int      CNENBbits.CNIEB13

#define ROT2_A_Pin      PORTBbits.RB12
#define ROT2_A_Dir      TRISBbits.TRISB12
#define ROT2_A_Int      CNENBbits.CNIEB12 
#define ROT2_B_Pin      PORTBbits.RB11
#define ROT2_B_Dir      TRISBbits.TRISB11
#define ROT2_B_Int      CNENBbits.CNIEB11 
#define ROT2_D_Pin      PORTBbits.RB10
#define ROT2_D_Dir      TRISBbits.TRISB10
#define ROT2_D_Int      CNENBbits.CNIEB10

#define ROT3_A_Pin      PORTAbits.RA2
#define ROT3_A_Dir      TRISAbits.TRISA2
#define ROT3_A_Int      CNENAbits.CNIEA2 
#define ROT3_B_Pin      PORTAbits.RA3
#define ROT3_B_Dir      TRISAbits.TRISA3
#define ROT3_B_Int      CNENAbits.CNIEA3 
#define ROT3_D_Pin      PORTAbits.RA4
#define ROT3_D_Dir      TRISAbits.TRISA4
#define ROT3_D_Int      CNENAbits.CNIEA4


#endif	/* XC_HEADER_TEMPLATE_H */

