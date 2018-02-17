#ifndef ADC_DRIVER_H
#define	ADC_DRIVER_H

    
extern uint16_t ADC_flag;


struct ADC_Buffer {
    int16_t value0;
    int16_t value1;
    int16_t value2;
    int16_t value3;
};
typedef struct ADC_Buffer AdcBuffer_t;

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

void D_ADC_Init(AdcBuffer_t * buffer);
void D_ADC_Enable(bool enable);


#endif