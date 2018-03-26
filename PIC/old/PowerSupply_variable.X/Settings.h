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
 *  DAC
 */
    
#define DAC_MODE        (FAST_MODE | PWR_ON)    /* DAC slow mode and power on */    
    
/**
 * SPI 2
 */    
    
#define SPI2_SCK_Pin    PORTBbits.RB10      /* SCK Pin on RP42-RB10           */
#define SPI2_SCK_Dir    TRISBbits.TRISB10
    
#define SPI2_SDO_Pin    PORTBbits.RB11      /* SDO Pin on RP43-RB11           */
#define SPI2_SDO_Dir    TRISBbits.TRISB11    
    
#define SPI2_SDI_Pin    PORTBbits.RB12      /* SDI Pin on RPI44-RB12          */
#define SPI2_SDI_Dir    TRISBbits.TRISB12    
    
#define SPI2_FS_Dir     TRISAbits.TRISA3   /* FS pin on RA3                   */
#define SPI2_FS_Pin     PORTAbits.RA3          

#define CS_MCP_Pin      PORTBbits.RB13      /* CS MCDP Pin on I/O-RB13        */
#define CS_MCP_Dir      TRISBbits.TRISB13      
    
#define CS_DAC_Pin      PORTBbits.RB4       /* CS DAC Pin on I/O-RB4          */
#define CS_DAC_Dir      TRISBbits.TRISB4    
    
#define SPI2_SDO_Map    0b001000
#define SPI2_SCK_Map    0b001001  
#define SPI2_SDI_Map    44

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

