#ifndef SPI_DRIVER_H
#define	SPI_DRIVER_H

#ifdef	__cplusplus
extern "C" {
#endif
    
extern uint16_t SPI2_ReadData;
    
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/    
    
/**
 *  Initializes the SPI2 module on with SPI ports defined in settings.h. SPI2
 *  will be a 16-bit module. SPI2 mode is Mode 0,0.
 */
void spi2Init();

/**
 * 
 * @param enable
 */
void spi2Enable(bool enable);

/**
 * 
 * @param data
 */
void spi2Write(uint16_t data);



#endif