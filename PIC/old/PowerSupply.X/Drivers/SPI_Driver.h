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

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

void D_SPI_Init();

void D_SPI_Enable(bool enable);

void D_SPI_Write(uint16_t data);



#endif