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
AdcBuffer_t bufferA;
AdcBuffer_t bufferB;

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
    AD1CON1bits.AD12B = 0;      // 10-Bit, 4-Channel ADC
    AD1CON1bits.FORM = 0b00;    // Unsigned integer
    AD1CON1bits.SSRCG = 0;      // See SSRC
    AD1CON1bits.SSRC = 0b010;   // Timer 3 compare ends sampling and starts conversion
    AD1CON1bits.ASAM = 1;       // Sampling begins immediately after last conversion. SAMP bit is auto-set.
    AD1CON1bits.ADDMABM = 0;    // DMA buffers are written in Scatter/Gather mode
    AD1CON1bits.SIMSAM = 0;
    
    /* AD1CON2 Register */
    AD1CON2bits.BUFM = 0;       // Always starts filling the buffer from the Start address
    AD1CON2bits.CSCNA = 1;      // Does not scan inputs
    AD1CON2bits.VCFG = 0b000;   // AVDD and AVSS reference pins
    AD1CON2bits.CHPS = 0b00;    // Convert CH0
    AD1CON2bits.SMPI = ADC_CHANNELS - 1;       // 4 ADC buffers
    AD1CON2bits.ALTS = 0;       // Alternate Input Sample Mode Select Bit
    
    /* AD1CON3 Register */
    AD1CON3bits.ADRC = 0;       // ADC Clock is derived from Systems Clock
    AD1CON3bits.ADCS = 139;     // ADC Conversion Clock Tad=Tcy*(ADCS+1)= (1/70M)*140 = 2µs (500Khz). 10-bit conversion takes 12-Tad = 24µs
    
    /* AD1CON4 Register*/
    AD1CON4bits.ADDMAEN = 1;    // Conversion results stored in ADC1BUFx registers, DMA is not used
    AD1CON4bits.DMABL = 3;      // Each buffer contains 16 words (2^DMABL)
    
    /* MUXA inputs */
    AD1CHS0bits.CH0SA = 0;      // MUXA +ve input selection (AIN0) for CH0
    AD1CHS0bits.CH0NA = 0;      // MUXA -ve input selection (Vref-) for CH0
    AD1CHS123bits.CH123SA = 0;  // MUXA +ve input selection (AIN0) for CH1
    AD1CHS123bits.CH123NA = 0;  // MUXA -ve input selection (Vref-) for CH0
    
    AD1CSSH = 0x0000;           //
    AD1CSSL = 0x000F;           // Scan AIN0, AIN1, AIN2, AIN3
    
    _AD1IF = 0;                 // Clear interrupt
    _AD1IE = 0;                 // Disable interrupts
    
    /* Timer 3*/
    T3CONbits.TON = 0;          // Disable timer
    T3CONbits.TCKPS = 0b01;     // 1:8 pre-scale -> T3 CLK = 70MH7 / 8 = 8.75MHz (114.28 ns))
    PR3 = 1750;                 // T3 period is PR3 * Tclk = 1750 * (1/8.75M) = 200µs
    TMR3 = 0x0000;              // Clear timer
    _T3IF = 0;                  // Clear interrupt flag
    _T3IE = 0;                  // Disable interrupt
    
    /* DMA */
    DMA1CONbits.AMODE = 0b10;   // Configure DMA for Peripheral Indirect mode
    DMA1CONbits.MODE = 0b10;    // Configure DMA for Continuous Ping-Ping mode
    DMA1PAD = (volatile unsigned int) &ADC1BUF0;    // Point DMA to ADC buffer
    DMA1CNT = (ADC_BUFFER_SIZE * ADC_CHANNELS) -1;  // # DMA requests
    DMA1REQ = 13;               // Select ADC1 as DMA request source
    
    DMA1STAL = (volatile unsigned int) &bufferA;        
    DMA1STAH = 0x0000;
    DMA1STBL = (volatile unsigned int) &bufferB;
    DMA1STBH = 0x0000;
    
    _DMA1IF = 0;                // Clear interrupt flag
    _DMA1IP = DMA1_IP;          // DMA interrupt priority
    _DMA1IE = 1;                // Enable DMA interrupt
    

}

void adcEnable(bool enable) {
    if (enable) {
        
        // Analog 
        ADC0_Asl = 1;
        ADC1_Asl = 1;
        ADC2_Asl = 1;
        ADC3_Asl = 1;
        
        // Input
        ADC0_Dir = 1;
        ADC1_Dir = 1;
        ADC2_Dir = 1;
        ADC3_Dir = 1;

        // Enable
        T3CONbits.TON = 1;      // Enable timer      
        AD1CON1bits.ADON = 1;   // Enable ADC  
        DMA1CONbits.CHEN = 1;   // Enable DMA
    } else {
        ANSELA = 0;             // All digital
        ANSELB = 0;
        T3CONbits.TON = 0;      // Disable timer
        AD1CON1bits.ADON = 0;   // Disable ADC
        DMA1CONbits.CHEN = 0;   // Disable DMA
    }
}

float adcValueToVoltage(uint16_t value) {
    return value * VREF / N;
}

/*******************************************************************************
 *          INTERRUPTS
 ******************************************************************************/

//  DMA done
uint16_t dmaBuffer = 0;
void __attribute__ ( (interrupt, no_auto_psv) ) _DMA1Interrupt(void) {
    
    // Switch between primary and secondary Ping-Pong buffers
    if (dmaBuffer == 0) {
        (*readDone)(bufferA);
    } else {
        (*readDone)(bufferB);
    }
    
    dmaBuffer ^= 1; // Toggle
    _DMA1IF = 0; // Clear
}
