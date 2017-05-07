/* 
 * File:   UART functions
 * Author: Wouter
 *
 */

#ifndef UART_CONTROLLER_H
#define	UART_CONTROLLER_H

#ifdef	__cplusplus
extern "C" {
#endif

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

   
#define MESSAGE_TYPE = 0;
#define BLOCK_TYPE = 1;
/**
 * Boolean indicating data can be read.
 */    
extern bool UART_flag;

/**
 * Data struct for reading data.
 */
typedef struct {
    uint8_t sender;
    const char* command;
    const char* message;
}READ_Data;
    

void C_UART_Init();

/**
 * Write data to the TX pin of UART module. 
 * @param command: Command
 * @param data: Data string to write
 */
void C_UART_Write(const char* command, const char* data);

/**
 * Write data to the TX pin of UART module. 
 * @param command: Command
 * @param data: Data integer to write
 */
void C_UART_WriteInt(const char* command, int data);

/**
 * Read data from the RX pin of UART module.
 * @return data: returns the data struct.
 */
READ_Data C_UART_Read();

/**
 * New data available
 * @param data
 */
void C_UART_NewData(uint8_t data);

/**
 * 
 * @return 
 */
READ_Data C_UART_ReadBlock(uint8_t cnt);
uint8_t C_UART_BlockLength();
uint8_t C_UART_MessageType();

void C_UART_Acknowledge(uint16_t val);

/**
 * Get the device name.
 * @return const char* device name
 */
const char* C_UART_GetDeviceName();

#endif