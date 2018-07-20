#ifndef SPI1_DRIVER_H
#define	SPI1_DRIVER_H

#include "../Settings.h"    
    
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/    
    
/**
 *  Initializes the SPI2 module on with SPI ports defined in settings.h. SPI2
 *  will be a 16-bit module. SPI2 mode is Mode 0,0.
 */
void spi1Init();

/**
 * 
 * @param enable
 */
void spi1Enable(bool enable);

/**
 * 
 * @param data
 */
uint8_t spi1Write(uint8_t data);



#endif