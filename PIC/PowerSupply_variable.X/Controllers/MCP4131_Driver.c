#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <math.h>

#include "../Drivers/SPI2_Driver.h"
#include "MCP4131_Driver.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
#define RAB         MCP_RESISTANCE
#define MAX_VALUE   (pow(2,MCP_RESOLUTION))
#define RS          (RAB / MAX_VALUE)

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/

/*******************************************************************************
 *          BASIC FUNCTIONS
 ******************************************************************************/
static void mcpWriteData(mcpData_t *data);

void mcpWriteData(mcpData_t *data) {
    uint16_t dataWord = 0;
    
    dataWord =  ((data->address << 12) & MCP_ADDR_MASK);
    dataWord |= ((data->command << 10) & MCP_COMMAND_MASK);
    dataWord |= ((data->writeData) & MCP_DATA_MASK);
    
    D_SPI2_Write(dataWord);
    while(!SPI2_ReadyToRead);
    SPI2_ReadyToRead = false;
    data->readData = SPI2_ReadData;
}

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/
void C_MCP4131_Init() {
    /* SPI2 Module */
    D_SPI2_Init();
}

void C_MCP4131_Enable(bool enable) {
    D_SPI2_Enable(enable);
}

void C_MCP4131_SetR(uint16_t value) {
    // Convert to 7-bit value (N))
    uint8_t N = 0;
    
    if (value > MCP_RW)  {
        double v1 = value - MCP_RW;
        double v2 = v1 / RAB;
        double v3 = v2 * MAX_VALUE;
        N = (uint8_t) (v3 + 0.5);
    } else {
        N = 0;
    }
    
    mcpData_t write;
    write.address = MCP_WIPER0_ADDR;
    write.command = WRITE;
    write.writeData = N;
    
    mcpWriteData(&write);
    
    // if ... check response for OK
}

/*******************************************************************************
 *          INTERRUPTS
 ******************************************************************************/


