#ifndef ADC_DRIVER_H
#define	ADC_DRIVER_H

#include "../Settings.h"

typedef struct ADC_Buffer {
    uint16_t adcCh0[ADC_BUFFER_SIZE];   // ADC channel 0
    uint16_t adcCh1[ADC_BUFFER_SIZE];   // ADC channel 1
    uint16_t adcCh2[ADC_BUFFER_SIZE];   // ADC channel 2
    uint16_t adcCh3[ADC_BUFFER_SIZE];   // ADC channel 3
} AdcBuffer_t;

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

void adcInit(void (*onAdcReadDone)(AdcBuffer_t data));
void adcEnable(bool enable);
float adcValueToVoltage(uint16_t value);


#endif