// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef SETTINGS_H
#define	SETTINGS_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "Controllers/DAC_Controller.h"

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * Interrupt priorities
 */

#define MI2C_IP     7
#define SPI2_IP     2
#define T3_IP       4    
#define ADC1_IP     3
#define U1RX_IP     1
#define IP_U1RX     5



    
/**
 *  PIC
 */
 
#define LED1            PORTBbits.RB15
#define LED1_Dir        TRISBbits.TRISB15
#define LED2            PORTBbits.RB14
#define LED2_Dir        TRISBbits.TRISB14
#define LED3            PORTBbits.RB13
#define LED3_Dir        TRISBbits.TRISB13
    
    
/**
 *  ADC
 */
    
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
    
    
/**
 * UART
 */    
#define UART1_BAUD      57600 /* 57600 */
#define UART1_ID        2   

#define UART1_RX_Dir    TRISBbits.TRISB6    /* RX Pin on RP38-RB6             */
#define UART1_TX_Dir    TRISBbits.TRISB7    /* TX Pin on RP39-RB7             */

#define UART1_RX_Map    0b0100110           /* RX on RP38                     */
#define UART1_TX_Map    RPOR2bits.RP39R     /* TX on RP39                     */
    
/**
 * SPI 2
 */    
    
#define SPI2_SCK_Pin    PORTBbits.RB11      /* SCK Pin on RP43-RB11           */
#define SPI2_SCK_Dir    TRISBbits.TRISB11
    
#define SPI2_SDO_Pin    PORTBbits.RB10      /* SDO Pin on RP42-RB10           */
#define SPI2_SDO_Dir    TRISBbits.TRISB10    
    
#define SPI2_SDI_Pin    PORTBbits.RB12      /* SDI Pin on RPI44-RB12          */
#define SPI2_SDI_Dir    TRISBbits.TRISB12      
    
#define SPI2_CS_Pin     PORTAbits.RA4       /* CS DAC Pin on I/O-RA4          */
#define SPI2_CS_Dir     TRISAbits.TRISA4    
    
#define SPI2_SDO_Map    0b001000
#define SPI2_SCK_Map    0b001001  
#define SPI2_SDI_Map    44
    
    
/**
 * I²C
 */
#define I2C_ADDRESS     0x03

#define I2C_SCL_Dir     TRISBbits.TRISB8        /* SCL Direction              */
#define I2C_SDA_Dir     TRISBbits.TRISB9        /* SDA Direction              */

#define I2C_SCL_Odc     ODCBbits.ODCB8          /* Open drain config for SCL  */
#define I2C_SDA_Odc     ODCBbits.ODCB9          /* Open drain config for SDA  */
    

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

