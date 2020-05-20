#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "SYSTEM_Driver.h"
/*******************************************************************************
 *          DEFINES
 ******************************************************************************/

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/
/* Prototype of the functions used in the file  */
void __attribute__ ( (__interrupt__) )  _OscillatorFail ( void );
void __attribute__ ( (__interrupt__) )  _AddressError( void );
void __attribute__ ( (__interrupt__) )  _StackError( void );
void __attribute__ ( (__interrupt__) )  _MathError( void );
void __attribute__ ( (__interrupt__) )  _DMACError( void );

/* Event function pointers */
static void (*trapError)(int16_t code);

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/
void sysInitOscillator(void) {
    /* Disable Watch Dog Timer */
    RCONbits.SWDTEN = 0;

    OSCCONbits.NOSC = 0b001; // Fast RC Oscillator (FRC) with divide-by-N and PLL (FRCPLL)
    OSCCONbits.CLKLOCK = 0; //bit 7 CLKLOCK: Clock Lock Enable bit
    OSCCONbits.IOLOCK = 0; //bit 6 IOLOCK: I/O Lock Enable bit
    OSCCONbits.OSWEN = 1; //bit 0 OSWEN: Oscillator Switch Enable bit
    
    while (OSCCONbits.COSC != 0b001);

    /* Reference clock on OSC2*/
    REFOCONbits.ROON = 1;   // Enable clock on output
    REFOCONbits.ROSEL = 0;  // System clock is used as the reference clock
    REFOCONbits.RODIV = 0b0000; // Reference clock not divided
}

void sysInitPll(void) {
    CLKDIVbits.ROI = 0; //bit 15 ROI: Recover on Interrupt bit
    CLKDIVbits.DOZE = 0;
    CLKDIVbits.FRCDIV = 0;
    CLKDIVbits.PLLPOST = 1; 
    CLKDIVbits.PLLPRE = 0; 
    PLLFBDbits.PLLDIV = 38; 
}

void sysInitPorts(void) {
    TRISA = 0x0000;
    ANSELA = 0x0000;
    PORTA = 0x0000;

    TRISB = 0x0000;
    ANSELB = 0x0000;
    PORTB = 0x0000;
    
}

void sysInitInterrupts(void) {
    INTCON1 = 0x0000;       // Clear all special pending flags
    INTCON3 = 0x0000;
    INTCON4 = 0x0000;

    _NSTDIS = 0; // Enable Interrupt Nesting   MUST !!!!
    return;
}

void sysInterruptEnable(bool enable) {
    if(enable) {
        _IPL = 0b000; // Set CPU at Level = 0
        _IPL3 = 0; // Lower priority
        _GIE = 1; // Global interrupt enable
    } else {    
        _IPL = 0b111; // Set CPU at Level = 7  (DISABLE ALL INTERRUPTS)
        _GIE = 0; // Global interrupt disable
    }
}

void sysReset(uint16_t afterDelay) {
    if (afterDelay > 0) {
        DelayMs(afterDelay);
    }
    asm ("RESET");
}

void sysInitError(void (*onError)(int16_t code)) {
    trapError = onError;
}

void __attribute__ ( (interrupt, no_auto_psv) ) _OscillatorFail( void ) {
    INTCON1bits.OSCFAIL = 0;        //Clear the trap flag
    if ((*trapError) != NULL) {
        (*trapError)(TRAP_OSC);
    }
}

void __attribute__ ( (interrupt, no_auto_psv) ) _AddressError( void ) {
    INTCON1bits.ADDRERR = 0;        //Clear the trap flag
    if ((*trapError) != NULL) {
        (*trapError)(TRAP_ADDR);
    }
}

void __attribute__ ( (interrupt, no_auto_psv) ) _StackError( void ) {
    INTCON1bits.STKERR = 0;         //Clear the trap flag
    if ((*trapError) != NULL) {
        (*trapError)(TRAP_STACK);
    }
}

void __attribute__ ( (interrupt, no_auto_psv) ) _MathError( void ) {
    INTCON1bits.MATHERR = 0;        //Clear the trap flag
    if ((*trapError) != NULL) {
        (*trapError)(TRAP_MATH);
    }
}

void __attribute__ ( (interrupt, no_auto_psv) ) _DMACError( void ) {
    INTCON1bits.DMACERR = 0;        //Clear the trap flag
    if ((*trapError) != NULL) {
        (*trapError)(TRAP_DMA);
    }
}

/*******************************************************************************
 End of File
*/
