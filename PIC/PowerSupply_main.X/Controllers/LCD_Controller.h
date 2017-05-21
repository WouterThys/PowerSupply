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
 */
void C_LCD_Init();

/**
 * 
 * @param enable
 */
void C_LCD_Enable(bool enable);

/**
 * 
 */
void C_LCD_Next();

/**
 * 
 */
void C_LCD_Select();

#endif	/* XC_HEADER_TEMPLATE_H */

