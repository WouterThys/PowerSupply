/* 
 * File:   GLCD_Controller.h
 * Author: wouter
 *
 * Created on April 7, 2020, 5:40 PM
 */

#ifndef GLCD_CONTROLLER_H
#define	GLCD_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "../Settings.h"
#include "../Drivers/SYSTEM_Driver.h"
#include "../Drivers/UART_Driver.h"
#include "../../Common/DATA_Definitions.h"


void glcdInit();

void glcdWriteMenu(const uint8_t menu);

void glcdSelectMenu(const uint8_t menu, bool selected);

void glcdSetVoltageState(const uint8_t menu, uint8_t state);

void glcdSetCurrentState(const uint8_t menu, uint8_t state);

void glcdSetVoltageSet(const uint8_t menu, const char * v);

void glcdSetCurrentSet(const uint8_t menu, const char * i);

void glcdSetVoltageRead(const uint8_t menu, const char * v);

void glcdSetCurrentRead(const uint8_t menu, const char * i);


#endif	/* GLCD_CONTROLLER_H */

