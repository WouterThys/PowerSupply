/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef LCD_DRIVER_H
#define	LCD_DRIVER_H

#include <xc.h> // include processor files - each processor file is guarded.  

/**
 * Initialize display with following default settings:
 *  Display off.
 *  Cursor off.
 *  Blinking off.
 */
void D_LCD_Init();

/**
 * Set the display on/off.
 * @param enable
 */
void D_LCD_Enable(bool enable);

/**
 * Clears the whole screen
 */
void D_LCD_ClearSreen();

/**
 * Clears one whole line. If the cursor was on that line, it will be put to 
 * zero.
 * @param line
 */
void D_LCD_ClearLine(uint8_t line);

/**
 * Write a string to the sceen.
 * @param str
 */
void D_LCD_WriteString(char * str);

/**
 * Write integer value to the screen.
 * @param value
 */
void D_LCD_WriteInt(int16_t value);

/**
 * Write double to the screen.
 * @param value
 * @param precision
 */
void D_LCD_WriteDouble(double value, uint16_t precision);

/**
 * 
 * @param line
 * @param pos
 */
void D_LCD_Goto(uint8_t line, uint8_t pos);

/**
 * 
 * @param on
 * @param blink
 */
void D_LCD_CursorStyle(bool on, bool blink);



#endif	/* XC_HEADER_TEMPLATE_H */

