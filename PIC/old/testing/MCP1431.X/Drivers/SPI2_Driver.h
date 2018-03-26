/* 
 * File:   UART functions
 * Author: Wouter
 *
 */

#ifndef SPI_DRIVER_H
#define	SPI_DRIVER_H

#ifdef	__cplusplus
extern "C" {
#endif
    
extern uint16_t SPI2_ReadData;
extern bool SPI2_ReadyToRead;
    
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/    
    
/**
 *  Initializes the SPI2 module on with SPI ports defined in settings.h. SPI2
 *  will be a 16-bit module. SPI2 mode is Mode 0,0.
 */
void D_SPI2_Init();

/**
 * 
 * @param enable
 */
void D_SPI2_Enable(bool enable);

/**
 * 
 * @param data
 */
void D_SPI2_Write(uint16_t data);



#endif