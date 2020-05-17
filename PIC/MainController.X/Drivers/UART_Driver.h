#ifndef UART_DRIVER_H
#define	UART_DRIVER_H


#define START_CHAR 0x24 /* $ */
#define STOP_CHAR  0x26 /* & */
#define SEP_CHAR   0x3A /* : */
#define ACK_CHAR   0x40 /* @ */
#define MES_CHAR   0x4D /* M */

#define STA_OK              0x00  /* Message converted successfull            */
#define STA_NOK             0x01  /* Message not converted                    */
#define STA_INVALID_START   0x02  /* Invalid start character                  */
#define STA_INVALID_STOP    0x04  /* Invalid stop character                   */
#define STA_INVALID_SEP     0x08  /* Invalid separator character              */
#define STA_INVALID_LENGTH  0x10  /* Total message length invalid             */
#define STA_INVALID_COMMAND 0x20  /* Command length invalid                   */
#define STA_INVALID_MESSAGE 0x40  /* Message length invalid                   */

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/
/**
 * Initialize the UART module, select which module to use. The module is enabled
 * when it is initialized.
 * @param baud: Baud rate of the UART 
 */
void uartDriverInit(uint16_t baud, void (*onReadDone)(uint8_t data));

/**
 * Enable the UART module
 * @param enable Enable or disable UART.
 */
void uartDriverEnable(bool enable);

/**
 * 
 * @param data
 */
void uartDriverWriteByte(uint8_t data);

/**
 * Write string to uart module
 * @param str: pointer to string to write
 * @param length: length of the string
 */
void uartDriverWriteString(const char * str, uint8_t length);

/**
 * Write data to the TX pin of UART module. 
 * @param command: Command
 * @param data: Data string to write
 */
void uartDriverWrite(char command, char* data);

/**
 * Write data to the TX pin of UART module. 
 * @param command: Command
 * @param data: Data integer to write
 */
void uartDriverWriteInt(char command, int data);


#endif
