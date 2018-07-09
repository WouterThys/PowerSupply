#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "SPI1_Driver.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static uint8_t readData;
static bool readDone;

/*******************************************************************************
 *          BASIC FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/
void spi1Init() {
    /* Disable SPI */
    spi1Enable(false);

    /* SPI1CON1 Register */
    SPI1CON1bits.DISSCK = 0;        // Internal SPI clock is enabled
    SPI1CON1bits.DISSDO = 0;        // SD01 pin is controlled by the module
    SPI1CON1bits.MODE16 = 0;        // Communication is byte-wide (8-bit)
    SPI1CON1bits.CKE = 1;           // Serial output data changes on transition from Idle clock state to active clock state
    SPI1CON1bits.CKP = 0;           // Idle state for clock is a low level; active state is a high level mode
    SPI1CON1bits.MSTEN = 1;         // Master mode
    SPI1CON1bits.SPRE = 0b001;      // Secondary pre-scale 2:1
    SPI1CON1bits.PPRE = 0b01;       // Primary pre-scale 4:1

    /* SPI1CON2 Register */
    SPI1CON2bits.FRMEN = 0;         // Framed SPI1 support is enabled (SSx pin used as frame sync pule input/output)
    SPI1CON2bits.SPIBEN = 0;        // Enhanced Buffer is disabled

    /* Interrupts */
    _SPI1IF = 0; // Clear flag
    _SPI1IP = SPI1_IP; // Priority
    _SPI1IE = 1; // Enable interrupt
}

void spi1Enable(bool enable) {
    if (enable) {
        // Ports
        SPI1_SDO_Dir = 0;           // SDO output   
        SPI1_SCK_Dir = 0;           // SCK output   
        SPI1_SDI_Dir = 1;           // SDI input
        
        SPI1STATbits.SPIEN = 1;     // Enable SPI1
    } else {
        SPI1_SDI_Dir = 0;           // SDI disable
        SPI1STATbits.SPIEN = 0;     // Disable 
    }
}

uint8_t spi1Write(uint8_t data) {
    readDone = false;
    SPI1BUF = data;
    while(!readDone);
    return readData;
}


/*******************************************************************************
 *          INTERRUPTS
 ******************************************************************************/

// SPI TX done
void __attribute__ ( (interrupt, no_auto_psv) ) _SPI1Interrupt(void) {
    if (_SPI1IF) {
        readData = SPI1BUF;
        readDone = true;
        _SPI1IF = 0;
    }
}

