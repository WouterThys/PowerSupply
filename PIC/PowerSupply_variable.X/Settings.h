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

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * Interrupt priorities
 */

#define MI2C_IP     7
#define SPI2_IP     2



    
/**
 *  PIC
 */
 
#define LED1            PORTBbits.RB15
#define LED1_Dir        TRISBbits.TRISB15
#define LED2            PORTBbits.RB14
#define LED2_Dir        TRISBbits.TRISB14
    
/**
 * SPI 2
 */    
    
#define SPI2_SCK_Pin    PORTBbits.RB10      /* SCK Pin on RP42-RB10           */
#define SPI2_SCK_Dir    TRISBbits.TRISB10
    
#define SPI2_SDO_Pin    PORTBbits.RB11      /* SDO Pin on RP43-RB11           */
#define SPI2_SDO_Dir    TRISBbits.TRISB11    
    
#define SPI2_SDI_Pin    PORTBbits.RB12      /* SDI Pin on RPI44-RB12          */
#define SPI2_SDI_Dir    TRISBbits.TRISB12    

#define SPI2_CS_Pin     PORTBbits.RB13      /* CS Pin on I/O-RB13             */
#define SPI2_CS_Dir     TRISBbits.TRISB13      
    
#define SPI2_SDO_Map    0b001000
#define SPI2_SCK_Map    0b001001  
#define SPI2_SDI_Map    44

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

