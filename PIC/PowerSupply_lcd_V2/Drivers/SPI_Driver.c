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
    SPI2CON1bits.DISSCK = 0;        // Internal SPI clock is enabled
    SPI2CON1bits.DISSDO = 0;        // SD02 pin is controlled by the module
    SPI2CON1bits.MODE16 = 0;        // Communication is byte-wide (8bits)
    SPI2CON1bits.SMP = 0;           // Input data is sampled at middle of data output time
    SPI2CON1bits.CKE = 0;           // Serial output data changes on transition from active clock state to Idle clock state (refer to bit 6)
    SPI2CON1bits.CKP = 1;           // Idle state for clock is a low level; active state is a high level
    SPI2CON1bits.SSEN = 0;          // SS2 pin is not used by module. Pin is controlled by port function
    SPI2CON1bits.MSTEN = 1;         // Master mode
    SPI2CON1bits.SPRE = 0;       // Secondary pre-scale 8:1
    SPI2CON1bits.PPRE = 0;       // Primary pre-scale 64:1
    
    /* SPI2CON2 Register */
    SPI2CON2bits.FRMEN = 0;         // Framed SPI2 support is disabled 
    SPI2CON2bits.SPIBEN = 0;        // Enhanced Buffer is disabled
    
    /* Port mapping */
    SPI2_SCK_Map = 0b001001;          // Assign SCK output pin
    SPI2_SDO_Map = 0b001000;          // Assign SDO output pin
    RPINR22bits.SDI2R = SPI2_SDI_Map; // Assign SDI input pin
    
    /* Interrupts */
    _SPI2IF = 0; // Clear flag
    _SPI2IP = 7; // Priority
    _SPI2IE = 1; // Enable
}

void D_SPI_Enable(bool enable) {
    if (enable) {
        SPI2_SS_Dir = 0;            // SS output    (RB13)
        SPI2_SDI_Dir = 1;           // SDI input    (RB12)
        SPI2_SDO_Dir = 0;           // SDO output   (RB11)
        SPI2_SCK_Dir = 0;           // SCK output   (RB10)
        
        SPI2_SS_Pin = 1;            // SS pin is active low
        SPI2STATbits.SPIEN = 1;     // Enable SPI2
    } else {
        SPI2STATbits.SPIEN = 0;     // Disable SPI2
    }
}

void D_SPI_Write(uint8_t data) {
    while(SPI2_SS_Pin == 0);
    SPI2_SS_Pin = 0;
    SPI2BUF = data;
}

// SPI TX done
void __attribute__ ( (interrupt, no_auto_psv) ) _SPI2Interrupt(void) {
    if (_SPI2IF) {
        SPI2_SS_Pin = 1;
        read = SPI2BUF;
        _SPI2IF = 0;
    }
}

