#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <math.h>

#include "../Settings.h"
#include "ADC_Driver.h"
#include "SYSTEM_Driver.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
#define N       pow(2, n)

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
AdcBuffer_t adcBuffer;

/*******************************************************************************
 *          BASIC FUNCTIONS
 ******************************************************************************/
static void (*readDone)(AdcBuffer_t data);

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/
void adcInit(void (*onAdcReadDone)(AdcBuffer_t data)) {

    /* Event function pointer */
    readDone = onAdcReadDone;
    
    /* Disable ADC */
    adcEnable(false);
    
    /* AD1CON1 Register */
    AD1CON1bits.ADON = 0;       // ADC is off
    AD1CON1bits.AD12B = 1;      // 12-bit, 1-channel ADC operation
    AD1CON1bits.FORM = 0b00;    // Unsigned integer
    AD1CON1bits.SSRCG = 0;      // See SSRC
    AD1CON1bits.SSRC = 0b010;   // Timer 3 compare ends sampling and starts conversion
    AD1CON1bits.ASAM = 1;       // Sampling begins immediately after last conversion. SAMP bit is auto-set.
    
    /* AD1CON2 Register */
    AD1CON2bits.VCFG = 0b000;   // AVDD and AVSS reference pins
    AD1CON2bits.CSCNA = 1;      // Scan inputs for CH0+ during Sample A bit
    AD1CON2bits.CHPS = 0b00;    // Converts CH0
    AD1CON2bits.SMPI = 0b00011; // Generates interrupt after completion of every 4th sample/conversion operation
    
    /* AD1CON3 Register */
    AD1CON3bits.ADRC = 1;       // ADC internal RC clock
    
    /* AD1CON4 Register*/
    AD1CON4bits.ADDMAEN = 0;    // Conversion results stored in ADC1BUFx registers, DMA is not used
    
    /* MUXA inputs */
    AD1CHS0bits.CH0SA = 0;      // CH0SA bits ignored for CH0 +VE input selection
    AD1CHS0bits.CH0NA = 0;      // Select VREF- for CH0 -VE input
    
    /* Input scan select registers */
    AD1CSSH = 0x0000;
    AD1CSSL = 0x000F;           // AN0, AN1, AN2, AN3
    
    /* Timer 3*/
    PR3 = 0x3FFF;
    T3CONbits.TON = 0;          // Disable timer
    T3CONbits.TCKPS = 0b10;     // 1:64 pre-scale
    
    /* Enable interrupts for ADC */
    _AD1IF = 0; // Clear flag
    _AD1IP = ADC1_IP; // Priority
    _AD1IE = 1; // Enable interrupts
}

void adcEnable(bool enable) {
    if (enable) {
        
        ANSELAbits.ANSA0 = 1;
        ANSELAbits.ANSA1 = 1;
        ANSELBbits.ANSB0 = 1;
        ANSELBbits.ANSB1 = 1;
        
        ADC0_Dir = 1;
        ADC1_Dir = 1;
        ADC2_Dir = 1;
        ADC3_Dir = 1;

        AD1CON1bits.ADON = 1;   // Enable ADC
        DelayUs(20);            // ADC stabilization delay 
        T3CONbits.TON = 1;      // Enable timer        
    } else {
        ANSELA = 0;             // All digital
        ANSELB = 0;
        T3CONbits.TON = 0;      // Disable timer
        AD1CON1bits.ADON = 0;   // Disable ADC
    }
}

float adcValueToVoltage(uint16_t value) {
    return value * VREF / N;
}

/*******************************************************************************
 *          INTERRUPTS
 ******************************************************************************/

//  ADC conversion done
void __attribute__ ( (interrupt, no_auto_psv) ) _AD1Interrupt(void) {
    if (_AD1IF) {
        adcBuffer.value0 = ADC1BUF0;
        adcBuffer.value1 = ADC1BUF1;
        adcBuffer.value2 = ADC1BUF2;
        adcBuffer.value3 = ADC1BUF3;
        _AD1IF = 0;
        adcEnable(false);      
        
        (*readDone)(adcBuffer);
    }
}
