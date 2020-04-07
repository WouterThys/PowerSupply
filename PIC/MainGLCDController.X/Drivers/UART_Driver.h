#ifndef UART_DRIVER_H
#define	UART_DRIVER_H


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../settings.h"


/*******************************************************************************
*           DEFINES
*******************************************************************************/


/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
typedef void (*readCallBack)(const uint8_t data);

/*******************************************************************************
*           FUNCTIONS
*******************************************************************************/

/**
 * Initialize uart module
 * @param baud: baud rate
 * @param inverted: invert data signals 
 */
void UART_Init(const uint16_t baud, const bool intverted, readCallBack callback);

/**
 * Send character
 * @param data: data to send
 */
void UART_PutChar(const char data);

/**
 * Send a string
 * @param data: buffer to send
 * @param length: length of the buffer
 */
void UART_PutString(const char *data, uint8_t length);

/**
 * To use with printf function
 */
void putch(unsigned char byte);

#endif	/* UART_DRIVER_H */

