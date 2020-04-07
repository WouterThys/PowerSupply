#ifndef DATA_CONTROLLER_H
#define	DATA_CONTROLLER_H

#include <stdint.h>
#include "../settings.h"
#include "../Drivers/UART_Driver.h"
#include "../../Common/DATA_Definitions.h"


typedef void (*dataCallback)(DataPacket_t * data);

/*******************************************************************************
 *          PROTOTYPES
 ******************************************************************************/

/**
 * Initialize with callback
 */
void dataInit(dataCallback callback);

/*
 * Read one byte
 */
void dataRead(uint8_t data);

#endif	/* DATA_CONTROLLER_H */

