/* 
 * File:   UART functions
 * Author: Wouter
 *
 */

#ifndef MCP4131_DRIVER_H
#define	MCP4131_DRIVER_H

#include <xc.h> // include processor files - each processor file is

#define MCP_WIPER0_ADDR  0x00
#define MCP_WIPER1_ADDR  0x01
#define MCP_TCON_ADDR    0x04
#define MCP_STATUS_ADDR  0x05

#define MCP_ADDR_MASK    0xF000
#define MCP_COMMAND_MASK 0x0C00
#define MCP_DATA_MASK    0x01FF

#define MCP_RESISTANCE   9630   /* Maximum resitance value is 10k Ohm         */
#define MCP_RESOLUTION   7      /* Resolution in bits of device               */
#define MCP_RW           114    /* Wiper resistance RW in Ohm                 */


typedef enum { 
    WRITE = 0,
    INCREMENT = 1, 
    DECREMENT = 2,
    READ = 3
} mcpCommand_t;

typedef struct {
    uint16_t     address;   /* Memory address to read/write (4-bit)           */
    mcpCommand_t command;   /* Command bits                                   */
    uint16_t     readData;  /* Data read back from SPI module                 */
    uint16_t     writeData; /* Data to write to SPI module                    */
} mcpData_t;
   
    
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/    
    
/**
 * 
 */
void C_MCP4131_Init();

/**
 * 
 * @param enable
 */
void C_MCP4131_Enable(bool enable);

/**
 * Set resistance value in Ohm.
 * @param value
 */
void C_MCP4131_SetR(uint16_t value);


#endif