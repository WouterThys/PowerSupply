#ifndef MENU_CONTROLLER_H
#define	MENU_CONTROLLER_H

#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "../Drivers/GLK19264A_Driver.h"
#include "../Bitmaps.h"


/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/ 
typedef enum SupplySelection {
    Voltage,
    Current,
    Temperature
} SupplySelection_e;
    
    
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

/**
 * Draw initial menu
 */
void menuInit(buttonCallback callback);

void menuSetId(uint8_t id);

void menuSetConnected(bool connected);

void menuSetEnabled(bool enabled);

void menuSetPidEnabled(bool enabled);

void menuSetSelection(SupplySelection_e selection);

void menuSetVoltageSet(uint16_t v);

void menuSetCurrentSet(uint16_t i);

void menuSetTemperatureSet(uint16_t t);

void menuSetVoltageMsr(uint16_t v);

void menuSetCurrentMsr(uint16_t i);

void menuSetTemperatureMsr(uint16_t t);

#endif	/* MENU_CONTROLLER_H */

