#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "../Drivers/SYSTEM_Driver.h"
#include "../Drivers/SPI1_Driver.h"
#include "../Settings.h"
#include "MCP_Registers.h"
#include "MCP_Controller.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
#define MCP_R_REGISTER      0x01
#define MCP_W_REGISTER      0x00

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/
#define McpWrite(d)     spi1Write(d); 

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/


/*******************************************************************************
 *          LOCAL FUNCTIONS
*******************************************************************************/
static void    MCP_WriteRegister(uint8_t reg, uint8_t value); // Write to a register
static uint8_t MCP_ReadRegister(uint8_t reg); // Read from a register
static void    MCP_Configure();



void MCP_WriteRegister(uint8_t reg, uint8_t value) {
    MCP_CS = 0; // Pull CS low
    
    McpWrite(MCP_ADDR + MCP_W_REGISTER); // Control byte 
    McpWrite(reg);                       // Address byte
    McpWrite(value);                     // Write data byte

    MCP_CS = 1; // Pull CS high
}

uint8_t MCP_ReadRegister(uint8_t reg) {
    uint8_t read = 0;
    
    MCP_CS = 0; // Pull CS low
    
    McpWrite(MCP_ADDR + MCP_R_REGISTER); // Control byte
    McpWrite(reg);                       // Address byte
    read = McpWrite(0x00);               // Read data byte

    MCP_CS = 1; // Pull CS high
    
    return read;
}


void MCP_Configure() {
    
    mcpIODIRA = 0x00; // Output
    mcpIODIRB = 0xFF; // Input
    MCP_WriteRegister(mcpIODIRAbits.address, mcpIODIRA);
    MCP_WriteRegister(mcpIODIRBbits.address, mcpIODIRB);
    
}

/*******************************************************************************
 *          CONTROLLER FUNCTIONS
*******************************************************************************/
void mcpInit() {
    
    // Initialize ports
    MCP_CS = 1;
    MCP_CS_Dir = 0;
    MCP_INTA_Dir = 1;
    MCP_INTB_Dir = 1;
    MCP_INTA_Pu = 1;
    MCP_INTB_Pu = 1;
    
    // Initialize SPI module
    spi1Init();
    spi1Enable(true);
    
    // Configure MCP module
    MCP_Configure();   
}

void mcpSetPORTA(uint8_t value) {
    mcpOLATA = value;
    MCP_WriteRegister(mcpOLATAbits.address, value);
}

void mcpSetPORTB(uint8_t value) {
    mcpOLATB = value;
    MCP_WriteRegister(mcpOLATBbits.address, value);
}

uint8_t mcpGetPORTA() {
    mcpGPIOA = MCP_ReadRegister(mcpGPIOAbits.address);
    return mcpGPIOA;
}

uint8_t mcpGetPORTB() {
    mcpGPIOB = MCP_ReadRegister(mcpGPIOBbits.address);
    return mcpGPIOB;
}
