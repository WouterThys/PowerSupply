#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "../Settings.h"
#include "../Drivers/SPI_Driver.h"
#include "DAC_Controller.h"

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

void C_DAC_Init() {
    D_SPI_Init();
}

void C_DAC_Enable(bool enable) {
    D_SPI_Enable(enable);
}

void C_DAC_Write(uint16_t data) { 
    D_SPI_Write((CTRL_MASK & (DAC_MODE << 12)) + (DATA_MASK & data));
}

