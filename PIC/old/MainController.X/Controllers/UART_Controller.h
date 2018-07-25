#ifndef UART_CONTROLLER_H
#define	UART_CONTROLLER_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "../../Common/COM_Settings.h"
#include "../Drivers/UART_Driver.h"

/**
 * 
 * @param onPutCommand
 */
void uartInit(bool (*onPutCommand)(Command_t data));

/**
 * Get the amount of data packages stored in the input buffer.
 * @return Amount of packages
 */
uint16_t uartBufferCount();

#endif	/* XC_HEADER_TEMPLATE_H */