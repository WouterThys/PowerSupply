/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef DAC_CONTROLLER_H
#define	DAC_CONTROLLER_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define FAST_MODE   0x0004  /* Speed mode 1 -> fast mode                      */
#define SLOW_MODE   0x0000  /* Speed mode 0 -> slow mode                      */
#define PWR_DOWN    0x0002  /* Power control 1 -> power down                  */
#define PWR_ON      0x0000  /* Power control 0 -> normal operation            */

#define CTRL_MASK   0xF000  /* Mask for 4-bit control bits                    */
#define DATA_MASK   0x0FFF  /* Mask for 12-bit data                           */

/**
 * 
 */
void C_DAC_Init();

/**
 * 
 * @param enable
 */
void C_DAC_Enable(bool enable);

/**
 * 
 * @param data
 */
void C_DAC_Write(uint16_t data);

#endif	/* XC_HEADER_TEMPLATE_H */

