#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <math.h>

#include "../Settings.h"
#include "../utils.h"
#include "SYSTEM_Driver.h"
#include "ADC_Driver.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
#define N       pow(2, ADC_n)

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
unsigned int BufferA[ADC_CHANNELS * ADC_BUFFER_SIZE] __attribute__( (space(ymemory)) );
unsigned int BufferB[ADC_CHANNELS * ADC_BUFFER_SIZE] __attribute__( (space(ymemory)) );

uint16_t dmaBuffer = 0;

/*******************************************************************************
 *          BASIC FUNCTIONS
 ******************************************************************************/
static void (*readDone)(uint16_t buffer, uint16_t * data);

static void initAdc1();
static void initTimer3();
static void initDma0();

void initAdc1() {

    // AD1CON1 register -> configuration
    AD1CON1bits.ADON = 0;
#ifdef ADC_n
    if (ADC_n == 12) {
        AD1CON1bits.AD12B = 1;      // 12-bit mode
    }
#endif
    AD1CON1bits.FORM = 0;       // Integer
    AD1CON1bits.ASAM = 1;       // Sampling begins immediately after the last conversion
    AD1CON1bits.SSRCG = 0;
    AD1CON1bits.SSRC = 0b010;   // Timer3 compare ends sampling and starts conversion
    AD1CON1bits.ADDMABM = 0;    // Scatter/Gather mode (Don't care -> DMA generates addresses)
    AD1CON1bits.SIMSAM = 0;     // Samples multiple channels individually in sequence
    
    // AD1CON2 register -> addressing
    AD1CON2bits.BUFM = 0;       // Always starts filling the buffer from the start address.
    AD1CON2bits.ALTS = 0;       // Always uses channel input selects for Sample MUXA
    AD1CON2bits.SMPI = ADC_CHANNELS-1;       // Increment rate is 3, 4 inputs are converted
    AD1CON2bits.CHPS = 0;       // Converts CH0
    AD1CON2bits.CSCNA = 1;      // Scans inputs for CH0+ during Sample MUXA
    
    // AD1CON3 register -> timing
    AD1CON3bits.ADRC = 0;       // System clock
    AD1CON3bits.ADCS = 139;     // ADC Conversion Clock Tad=Tcy*(ADCS+1)= (1/70M)*140 = 2µs (500Khz)
            
    // AD1CON4 register -> DMA
    AD1CON4bits.ADDMAEN = 1;    // Conversion results are stored in the ADC1BUF0 register for transfer to RAM using DMA
    AD1CON4bits.DMABL = Log2(ADC_BUFFER_SIZE);      // Allocates 8 words of buffer to each analog input
    
    // Input select
    AD1CHS0bits.CH0NA = 0;      // MUXA -ve input selection (Vref-) for CH0
    AD1CHS0bits.CH0SA = 0;      // 
    AD1CHS123bits.CH123SA = 0;  // MUXA +ve input selection (AIN0) for CH1
    AD1CHS123bits.CH123NA = 0;  // MUXA -ve input selection (Vref-) for CH1
    
    // Channel scan select
    AD1CSSH = 0x0000;           // No channel scan
    AD1CSSL = 0x000F;           // AN0, AN1, AN2, AN3
    
    // Interrupts
    _AD1IF = 0;              // Clear flag
    _AD1IE = 0;              // No interrupts
}

// Time out every 400µs 
void initTimer3() {
    
    T3CONbits.TCKPS = 1;    // 1:8 -> 8.75 MHz
    PR3 = 3500;             // 1/8.75MHz * 35 00= 400µs
    
    TMR3 = 0x0000;          // Clear register
    _T3IF = 0;              // Clear flag
    _T3IE = 0;              // No interrupts
}

void initDma0() {
    // DMA0CON registers
    DMA0CONbits.AMODE = 2;  // Peripheral indirect with Post-Increment 
    DMA0CONbits.MODE = 2;   // Configure DMA for Continuous Ping-Pong mode
    
    // Tie it to ADC1
    DMA0PAD = (volatile uint16_t) &ADC1BUF0;
    DMA0CNT = 31;           // 32 DMA request
    DMA0REQ = 13;           // ADC1 as DMA request source
    
    DMA0STAL = (volatile uint16_t) &BufferA;
    DMA0STAH = 0x0000;

    DMA0STBL = (volatile uint16_t) &BufferB;
    DMA0STBH = 0x0000;
   
    // Interrupts
    _DMA0IF = 0;    // Clear the DMA interrupt flag bit
    _DMA0IE = 1;    // Set the DMA interrupt enable bit
}

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/
void adcDriverInit(void (*onAdcReadDone)(uint16_t buffer, uint16_t * data)) {

    /* Event function pointer */
    readDone = onAdcReadDone;
    
    /* Disable ADC */
    adcDriverEnable(false);
    
    initAdc1();
    initDma0();
    initTimer3();
}

void adcDriverEnable(bool enable) {
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
        AD1CON1bits.ADON = 1;   // Enable ADC  
        DMA0CONbits.CHEN = 1;   // Enable DMA
        T3CONbits.TON = 1;      // Enable timer      
    } else {
        ANSELA = 0;             // All digital
        ANSELB = 0;
        T3CONbits.TON = 0;      // Disable timer
        AD1CON1bits.ADON = 0;   // Disable ADC
        DMA0CONbits.CHEN = 0;   // Disable DMA
    }
}

/*******************************************************************************
 *          INTERRUPTS
 ******************************************************************************/

//  DMA done
void __attribute__ ( (interrupt, no_auto_psv) ) _DMA0Interrupt(void) {
    // Switch between primary and secondary Ping-Pong buffers
    uint16_t c;
    if (dmaBuffer == 0) {
        for (c = 0; c < ADC_CHANNELS; c++) {
            (*readDone)(c, &BufferA[(c * ADC_BUFFER_SIZE)]);
        }
    } else {
        for (c = 0; c < ADC_CHANNELS; c++) {
            (*readDone)(c, &BufferB[(c * ADC_BUFFER_SIZE)]);
        }
    }
    
    dmaBuffer ^= 1; // Toggle
    _DMA0IF = 0; // Clear
}