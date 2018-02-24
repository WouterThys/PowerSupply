/* 
 * File:   UART functions
 * Author: Wouter
 *
 */

#ifndef UART_DRIVER_H
#define	UART_DRIVER_H

#ifdef	__cplusplus
extern "C" {
#endif

#define UART_MODULE_1   0
#define UART_MODULE_2   1
    
#define MESSAGE_TYPE = 0;
#define BLOCK_TYPE = 1;
    
/**
 * Boolean indicating data can be read.
 */    
extern bool UART_flag;

/**
 * Data Struct for reading data.
 */
typedef struct {
    uint8_t sender;
    char command[3];
    char message[10];
} READ_Data_t;

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/
/**
 * Initialize the UART module, select which module to use. The module is enabled
 * when it is initialized.
 * @param which: UART_MODULE_1 or UART_MODULE_2
 * @param baud: Baud rate of the UART 
 */
void uartInit(uint16_t which, uint16_t baud);

/**
 * Enable the UART module
 * @param which: UART_MODULE_1 or UART_MODULE_2
 * @param enable Enable or disable UART.
 */
void uartEnable(uint16_t which, bool enable);

/**
 * 
 * @param data
 */
void uartWriteByte(uint8_t data);

/**
 * 
 * @return 
 */
uint8_t uartReadByte(void);







/**
 * Write data to the TX pin of UART module. 
 * @param command: Command
 * @param data: Data string to write
 */
void uartWrite(const char* command, const char* data);

/**
 * Write data to the TX pin of UART module. 
 * @param command: Command
 * @param data: Data integer to write
 */
void uartWriteInt(const char* command, int data);

/**
 * Read data from the RX pin of UART module.
 * @return data: returns the data struct.
 */
READ_Data_t uartRead();

/**
 * New data available
 * @param data
 */
void uartNewData(uint8_t data);

/**
 * 
 * @return 
 */
READ_Data_t uartReadBlock(uint8_t cnt);
uint8_t uartBlockLength();
uint8_t uartMessageType();

void uartAcknowledge(uint16_t val);

/**
 * Get the device name.
 * @return const char* device name
 */
const char* uartGetDeviceName();


#endif