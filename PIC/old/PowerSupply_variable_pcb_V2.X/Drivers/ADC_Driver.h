#ifndef ADC_DRIVER_H
#define	ADC_DRIVER_H

#include "../Settings.h"

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

void adcInit(void (*onAdcReadDone)(uint16_t buffer, uint16_t * data));
void adcEnable(bool enable);
float adcValueToVoltage(uint16_t value);


#endif