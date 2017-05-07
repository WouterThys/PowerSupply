#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "../Settings.h"
#include "SPI_Driver.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/

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
    SPI2CON1bits.MODE16 = 1;        // Communication is word-wide (16-bit)
    SPI2CON1bits.CKE = 0;           // Serial output data changes on transition from idle clock state to active clock state
    SPI2CON1bits.CKP = 0;           // Idle state for click is a low level, active state is a high level
    SPI2CON1bits.SSEN = 1;          // SS2 pin is used for Slave mode
    SPI2CON1bits.MSTEN = 1;         // Master mode
    SPI2CON1bits.SPRE = 0b00;       // Secondary pre-scale 8:1
    SPI2CON1bits.PPRE = 0b10;       // Primary pre-scale 4:1
    
    /* SPI2CON2 Register */
    SPI2CON2bits.FRMEN = 1;         // Framed SPI2 support is enabled (SSx pin used as frame sync pule input/output)
    SPI2CON2bits.SPIFSD = 0;        // Frame sync pulse output (master)
    SPI2CON2bits.FRMPOL = 0;        // Frame sync pulse is active-low
    SPI2CON2bits.FRMDLY = 0;        // Frame sync pulse precedes first bit clock
    SPI2CON2bits.SPIBEN = 0;        // Enhanced Buffer is disabled
    
}

void D_SPI_Enable(bool enable) {
    if (enable) {
        SPI2_FS_Dir = 0;            // FS output    (RB9)
        SPI2_SDO_Dir = 0;           // SDO output   (RB10)
        SPI2_SCK_Dir = 0;           // SCK output   (RB11)
        
        RPOR4bits.RP42R = SPI2_SCK_Map;
        RPOR4bits.RP43R = SPI2_SDO_Map;
        RPOR3bits.RP41R = SPI2_FS_Map;
        
        SPI2STATbits.SPIEN = 1;     // Enable SPI2
    } else {
        SPI2STATbits.SPIEN = 0;     // Disable SPI2
    }
}

void D_SPI_Write(uint16_t data) {
     SPI2BUF = data;
}

