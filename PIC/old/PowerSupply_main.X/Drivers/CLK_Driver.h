/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef CLK_DRIVER_H
#define	CLK_DRIVER_H

#include <xc.h> // include processor files - each processor file is guarded.  

extern bool CLK_Flag;

/**
 * 
 */
void D_CLK_Init();

/**
 * 
 * @param enable
 */
void D_CLK_Enable(bool enable);

#endif	/* XC_HEADER_TEMPLATE_H */

