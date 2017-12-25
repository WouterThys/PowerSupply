#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "../Settings.h"
#include "../Drivers/SPI2_Driver.h"
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
    D_SPI2_Init();
    
    /* Ports*/
    CS_DAC_Dir = 0; // Output
    CS_DAC_Pin = 1; // Active low chip select
    
    SPI2_FS_Dir = 0;
    SPI2_FS_Pin = 1;
}

void C_DAC_Enable(bool enable) {
    D_SPI2_Enable(enable);
    CS_DAC_Pin = 1; // Active low chip select
    SPI2_FS_Pin = 1;
}

void C_DAC_Write(uint16_t data) { 
    while(CS_MCP_Pin == 0 || CS_DAC_Pin == 0); // Wait while still writing previous data
    CS_DAC_Pin = 0;
    SPI2_FS_Pin = 0;
    
    D_SPI2_Write((CTRL_MASK & (DAC_MODE << 12)) + (DATA_MASK & data));
    
//    while(!SPI2_ReadyToRead);
//    SPI2_ReadyToRead = false;
//    SPI2_FS_Pin = 1;
}

