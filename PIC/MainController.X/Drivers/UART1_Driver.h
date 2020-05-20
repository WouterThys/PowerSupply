#ifndef UART_DRIVER1_H
#define	UART_DRIVER1_H

#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "../Settings.h"
#include "SYSTEM_Driver.h"

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/
/**
 * Initialize the UART module, select which module to use. The module is enabled
 * when it is initialized.
 * @param baud: Baud rate of the UART 
 */
void uart1DriverInit(uint16_t baud, void (*onReadDone)(uint8_t data));

/**
 * Enable the UART module
 * @param enable Enable or disable UART.
 */
void uart1DriverEnable(bool enable);

/**
 * 
 * @param data
 */
void uart1DriverWrite(uint8_t data);


#endif
