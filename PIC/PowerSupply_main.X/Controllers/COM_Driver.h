/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef COM_DRIVER_H
#define	COM_DRIVER_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "../Drivers/ENC_Driver.h"
#include "../../Common/Drivers/I2C_Driver.h"
#include "../../Common/I2C_Settings.h"
#include "../../Common/COM_Settings.h"


/**
 * 
 */
void C_COM_Init();

/**
 * 
 * @param enable
 */
void C_COM_Enable(bool enable);

/**
 * 
 */
void C_COM_I2CReset();

/**
 * 
 * @param menuId
 * @param subMenuId
 * @return I2C result code
 */
int16_t C_COM_LcdDraw(uint8_t menuId, uint8_t subMenuId);

/**
 * 
 * @param what
 * @param select
 * @param id
 * @return I2C result code
 */
int16_t C_COM_LcdSelect(uint8_t what, bool select, uint8_t id);

/**
 * 
 * @param id
 * @param value
 * @return I2C result code
 */
int16_t C_COM_LcdSet(uint8_t id, int16_t value);


#endif	/* XC_HEADER_TEMPLATE_H */

