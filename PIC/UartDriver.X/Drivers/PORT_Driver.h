/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/
/**
 * Sets the port directions and mapping for the SPI module. Select the module
 * to use.
 * @param which: SPI_MODULE_1 or SPI_MODULE_2
 * @param MS: SPI_MASTER or SPI_SLAVE
 */
void D_PORT_SetSpiPorts(uint16_t which, uint16_t MS);
/**
 * Sets the port directions and mapping for the UART module. Select the module
 * to use.
 * @param which: UART_MODULE_1 or UART_MODULE_2
 */
void D_PORT_SetUartPorts(uint16_t which);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

