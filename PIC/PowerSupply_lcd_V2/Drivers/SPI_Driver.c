#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "../Settings.h"
#include "INTERRUPT_Driver.h"
#include "SPI_Driver.h"
#include "SYSTEM_Driver.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static uint16_t read;

/*******************************************************************************
 *          BASIC FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/
void D_SPI_Init() {
    /* Disable SPI */
    D_SPI_Enable(false);
    
    /* SPI2CON1 Register */
    SPI1CON1bits.DISSCK = 0;        // Internal SPI clock is enabled
    SPI1CON1bits.DISSDO = 0;        // SD02 pin is controlled by the module
    SPI1CON1bits.MODE16 = 0;        // Communication is byte-wide (8bits)
    SPI1CON1bits.SMP = 0;           // Input data is sampled at middle of data output time
    SPI1CON1bits.CKE = 0;           // Serial output data changes on transition from active clock state to Idle clock state (refer to bit 6)
    SPI1CON1bits.CKP = 1;           // Idle state for clock is a low level; active state is a high level
    SPI1CON1bits.SSEN = 0;          // SS2 pin is not used by module. Pin is controlled by port function
    SPI1CON1bits.MSTEN = 1;         // Master mode
    SPI1CON1bits.SPRE = 0;       // Secondary pre-scale 8:1
    SPI1CON1bits.PPRE = 0;       // Primary pre-scale 64:1
    
    /* SPI2CON2 Register */
    SPI1CON2bits.FRMEN = 0;         // Framed SPI2 support is disabled 
    SPI1CON2bits.SPIBEN = 0;        // Enhanced Buffer is disabled
    
    /* Interrupts */
    _SPI1IF = 0; // Clear flag
    _SPI1IP = 7; // Priority
    _SPI1IE = 1; // Enable
}

void D_SPI_Enable(bool enable) {
    if (enable) {
        SPI1_SS_Dir = 0;            // SS output    (RB10)
        SPI1_SDI_Dir = 1;           // SDI input    (RB9)
        SPI1_SDO_Dir = 0;           // SDO output   (RB8)
        SPI1_SCK_Dir = 0;           // SCK output   (RB7)
        
        SPI1_SS_Pin = 1;            // SS pin is active low
        SPI1STATbits.SPIEN = 1;     // Enable SPI2
    } else {
        SPI1STATbits.SPIEN = 0;     // Disable SPI2
    }
}

void D_SPI_Write(uint8_t data) {
    while(SPI1_SS_Pin == 0);
    SPI1_SS_Pin = 0;
    SPI1BUF = data;
}

// SPI TX done
void __attribute__ ( (interrupt, no_auto_psv) ) _SPI1Interrupt(void) {
    if (_SPI1IF) {
        SPI1_SS_Pin = 1;
        read = SPI1BUF;
        _SPI1IF = 0;
    }
}

