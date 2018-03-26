/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef LCD_CONTROLLER_H
#define	LCD_CONTROLLER_H

#include <xc.h> // include processor files - each processor file is guarded.  

/**
 * 
 */
void C_LCD_Init();

/**
 * 
 * @param id
 */
void C_LCD_DrawMenu(uint8_t id);

/**
 * 
 * @param id
 */
void C_LCD_DrawSubMenu(uint8_t id);

/**
 * 
 * @param what
 * @param id
 * @param select
 */
void C_LCD_SetSelected(uint8_t what, uint8_t id, bool select);

/**
 * 
 * @param id
 * @param value
 */
void C_LCD_SetFieldValue(uint8_t id, int16_t value);

#endif	/* XC_HEADER_TEMPLATE_H */

