/* 
 * File:   UART functions
 * Author: Wouter
 *
 */

#ifndef UART_DRIVER_H
#define	UART_DRIVER_H

#ifdef	__cplusplus
extern "C" {
#endif

#define UART_MODULE_1   0
#define UART_MODULE_2   1

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/
/**
 * Initialize the UART module, select which module to use. The module is enabled
 * when it is initialized.
 * @param which: UART_MODULE_1 or UART_MODULE_2
 * @param baud: Baud rate of the UART 
 */
void D_UART_Init(uint16_t which, uint16_t baud);

/**
 * Enable the UART module
 * @param which: UART_MODULE_1 or UART_MODULE_2
 * @param enable Enable or disable UART.
 */
void D_UART_Enable(uint16_t which, bool enable);

/**
 * 
 * @param data
 */
void D_UART_WriteByte(uint8_t data);

/**
 * 
 * @return 
 */
uint8_t D_UART_ReadByte(void);


#endif