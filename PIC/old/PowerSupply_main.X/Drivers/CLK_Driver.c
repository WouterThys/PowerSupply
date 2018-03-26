#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>

#include "../Settings.h"
#include "CLK_Driver.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
bool CLK_Flag = false;
static uint16_t clkCnt = 0;
/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/

void D_CLK_Init() {
    /* Variables */
    clkCnt = 0;
    
   /* T2CON register */
    T2CONbits.TON = 0; // Disable
    T2CONbits.T32 = 1; // Timer x (T2) and Timer y (T3) form a single 32-bit timer
    T2CONbits.TCKPS = 0b00; // 1:1
    
    TMR3 = 0x0000;
    TMR2 = 0x0000;
    
    PR3 = 0x000D;
    PR2 = 0xFFFF;
    
    /* Interrupts */
    _T3IF = 0; // Clear flag
    _T3IP = T2_IP; // Priority
    _T3IE = 0; //Wait
}

void D_CLK_Enable(bool enable) {
    if (enable) {
        clkCnt = 0;
        
        CLK_Dir = 0; // Output
        _T3IE = 1;
        T2CONbits.TON = 1;
    } else {
        T2CONbits.TON = 0;
        _T3IE = 0;
    }
}


/*******************************************************************************
 *          INTERRUPTS
 ******************************************************************************/
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void) {
    if (_T3IF) {
        _T3IF = 0;
        CLK_Pin = !CLK_Pin;
        clkCnt ++;
        
        if (clkCnt >= 8) {
            clkCnt = 0;
            CLK_Flag = true; 
        }
        
    }
}



