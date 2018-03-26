/* 
 * File:   UART functions
 * Author: Wouter
 *
 */

#ifndef ADC_DRIVER_H
#define	ADC_DRIVER_H

#include "../../Common/Drivers/I2C_Driver.h"
    
extern uint16_t ADC_flag;

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/
void D_ADC_Init(i2cAnswer_t * buffer);
void D_ADC_Enable(bool enable);


#endif