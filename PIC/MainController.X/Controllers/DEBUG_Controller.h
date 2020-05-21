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

void dbgPrintSupplyStatus(uint8_t id, SupplyStatus_t status);

void dbgPrintSupplyMeasurements(uint8_t id, uint16_t v, uint16_t i, uint16_t t);

void dbgPrintI2CError(uint16_t id, int16_t errorCode);

void  dbgPrintI2cData(uint16_t i, uint8_t data);

void dbgPrintLcdCallback(uint8_t data);

#endif	

