// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef SETTINGS_H
#define	SETTINGS_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "Controllers/DAC_Controller.h"

#define DEBUG       1           /* General debug enable                       */
#define DEBUG_I2C  (1 & DEBUG)  /* I²C debug enable                           */
#define DEBUG_FSM  (1 & DEBUG)  /* FSM debug enable                           */
    
/**
 * Board constants
 */    
#define Rs          0.1     /* Sense resistance                               */
#define Rt          68100   /* Temperature resitance                          */
#define Igain       20      /* Current sense amplifier gain                   */
#define Vgain       5       /* Voltage amplification from DAC to LT3080       */
#define Vdivider    1/5     /* Voltage divider for sensing output voltage     */
    
/**
 * Interrupt priorities
 */

#define IP_I2C     7
#define T3_IP       6    
#define DMA1_IP     5

#define IP_U1RX     3
#define SPI2_IP     2
#define IP_CN       1

    
/**
 *  PIC
 */
 
#define LED1            PORTBbits.RB15
#define LED1_Dir        TRISBbits.TRISB15
#define LED2            PORTBbits.RB14
#define LED2_Dir        TRISBbits.TRISB14
    
#define CLIP_PIN        PORTBbits.RB5    
#define CLIP_PIN_Dir    TRISBbits.TRISB5
#define CLIP_PIN_Cn     CNENBbits.CNIEB5
#define CLIP_LED_Dir    TRISBbits.TRISB13
#define CLIP_LED        PORTBbits.RB13
    
/**
 *  ADC
 */
#define VREF            2.048
#define n               10
    
#define ADC_BUFFER_SIZE 8
#define ADC_CHANNELS    4      
    
#define ADC_CURRENT     0
#define ADC_TEMPERATURE 1    
  
#define ADC0_Asl        ANSELAbits.ANSA0        
#define ADC0_Pin        PORTAbits.RA0
#define ADC0_Dir        TRISAbits.TRISA0
#define ADC0_PNr        1 /* AN0 */
  
#define ADC1_Asl        ANSELAbits.ANSA1           
#define ADC1_Pin        PORTAbits.RA1
#define ADC1_Dir        TRISAbits.TRISA1
#define ADC1_PNr        2 /* AN1 */

#define ADC2_Asl        ANSELBbits.ANSB0      
#define ADC2_Pin        PORTBbits.RB0
#define ADC2_Dir        TRISBbits.TRISB0
#define ADC2_PNr        3 /* AN2 */

#define ADC3_Asl        ANSELBbits.ANSB1       
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
#define I2C_SLAVE       /* Or I2C_MASTER                                       */
#define I2C_WORD_WIDE   /* All data is send as 16-bit                         */
#define I2C_ADDRESS     0x55 

#define I2C_SCL_Dir     TRISBbits.TRISB8        /* SCL Direction              */
#define I2C_SDA_Dir     TRISBbits.TRISB9        /* SDA Direction              */

#define I2C_SCL_Odc     ODCBbits.ODCB8          /* Open drain config for SCL  */
#define I2C_SDA_Odc     ODCBbits.ODCB9          /* Open drain config for SDA  */
    

#endif	/* XC_HEADER_TEMPLATE_H */

