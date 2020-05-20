#ifndef DEBUG_CONTROLLER_H_
#define	DEBUG_CONTROLLER_H_

#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "../../Common/CommonData.h"
#include "../Drivers/I2C_Driver.h"

void dbgInit();

void dbgPrintSupplyStatus(SupplyStatus_t status);

void dbgPrintI2CError(int16_t errorCode);

void  dbgPrintI2cData(uint16_t i, uint8_t data);

#endif	

