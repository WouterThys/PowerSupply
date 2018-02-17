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

typedef struct {
    uint16_t A_B  : 1;  // DAC_A or DAC_B select (0 = write to A, 1 = write to B)
    uint16_t BUF  : 1;  // V_REF Input Buffer Control bit ( 0 = unbuffered, 1 = buffered)
    uint16_t GA   : 1;  // Output Gain Select bit (0 = 2x, 1 = 1x);
    uint16_t SHDN : 1;  // Output Power Down Control bit (0 = Output buffer disabled, 1 = Output buffer enabled)
    uint16_t data : 12; // DAC Data bits (D {0:4095})
} DAC_Command_t; 

/**
 * 
 */
void C_DAC_Init();

/**
 * 
 * @param enable
 */
void C_DAC_Enable(bool enable);

void C_DAC_InitDacA(bool buf, uint16_t gain);
void C_DAC_InitDacB(bool buf, uint16_t gain);

void C_DAC_ShutDownA();
void C_DAC_ShutDownB();

void C_DAC_SetVoltageA(float voltage);
void C_DAC_SetVoltageB(float voltage);



#endif	/* XC_HEADER_TEMPLATE_H */

