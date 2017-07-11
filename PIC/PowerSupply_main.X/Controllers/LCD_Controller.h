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
#include "../Headers/Menu.h"

/**
 */
void C_LCD_Init(cursor_t *cursor);

/**
 * 
 * @param enable
 */
void C_LCD_Enable(bool enable);

/**
 * 
 * @param cursor
 * @param encoderValues
 */
void C_LCD_Turn(cursor_t *cursor, enc_t encoderValues);

/**
 * 
 * @param cursor
 */
void C_LCD_Press(cursor_t *cursor);

/**
 * 
 * @param fieldId
 * @param value
 */
void C_LCD_SetValue(uint8_t fieldId, cursor_t *cursor, int16_t value);

#endif	/* XC_HEADER_TEMPLATE_H */

