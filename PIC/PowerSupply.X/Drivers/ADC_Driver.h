/* 
 * File:   UART functions
 * Author: Wouter
 *
 */

#ifndef ADC_DRIVER_H
#define	ADC_DRIVER_H

#ifdef	__cplusplus
extern "C" {
#endif
    
extern uint16_t ADC_flag;
extern uint16_t ADC_Values[4];

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/
void D_ADC_Init();
void D_ADC_Enable(bool enable);


#endif