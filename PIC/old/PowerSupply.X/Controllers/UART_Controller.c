#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "../Settings.h"
#include "../Drivers/UART_Driver.h"
#include "../Drivers/INTERRUPT_Driver.h"
#include "UART_Controller.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/

// &
#define START_CHAR 0x26 
// $
#define STOP_CHAR  0x24 
// :
#define SEP_CHAR   0x3A 
// A
#define ACK_CHAR   0x41
// M
#define MES_CHAR   0x4D

#define READ_STATE_START   0x00
#define READ_STATE_ID      0x01
#define READ_STATE_LENGTH  0x02
#define READ_STATE_COMMAND 0x03
#define READ_STATE_MESSAGE 0x04
#define READ_STATE_ACK_ID  0x05
#define READ_STATE_CHECK   0x06
#define READ_STATE_END     0x07

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static uint8_t deviceId;

const char* messageCharacter =     "M";
const char* blockCharacter =       "B";

typedef struct {
    char command[3];      // Command buffer
    uint8_t commandCnt;   // Count of command buffer
    char message[10];  // Message buffer
    uint8_t messageCnt;   // Count of the message buffer
} READ_ComMes;

typedef struct {
    uint8_t id;     // Type of the message buffer  
    uint8_t blockLength;  // One block = <Command>:<Message>:
    READ_ComMes comMes[3];   // Array with commands and messages
    uint8_t ackId;       // Id send from the sender, to acknowledge
    
    uint8_t check;    // Value to check
    uint8_t state;    // State of the read buffer
} READ_Buffer;
READ_Buffer readBuffer;

READ_Data readData;

bool UART_flag;
static bool canWrite;

/*******************************************************************************
 *          BASIC FUNCTIONS
 ******************************************************************************/
static void fillDataBuffer(uint8_t data);
static void acknowledge(uint8_t ackId);

READ_ComMes comMes;
uint8_t bufferCnt = 0;
uint8_t blockLength = 0;

void fillDataBuffer(uint8_t data){
    switch(readBuffer.state) {
        case READ_STATE_START:
            if(data == START_CHAR) {
                readBuffer.id = 0;
                readBuffer.blockLength = 0;
                comMes.messageCnt = 0;
                comMes.commandCnt = 0;
                blockLength = 0;
                bufferCnt = 0;
                UART_flag = false;
                readBuffer.state = READ_STATE_ID;
            } else {
                readBuffer.state = READ_STATE_START;
                return;
            }
            break;
        
        case READ_STATE_ID:
            if (data == SEP_CHAR) {
                readBuffer.state = READ_STATE_LENGTH;
            } else {
                readBuffer.id = data;
            }
            break;
            
        case READ_STATE_LENGTH:
            if (data == SEP_CHAR) {
                readBuffer.state = READ_STATE_COMMAND;
            } else {
                readBuffer.blockLength = (data - 0x30);
                blockLength = (data - 0x30);
            }
            break;
            
        case READ_STATE_COMMAND:
            if (data == SEP_CHAR) {
                readBuffer.state = READ_STATE_MESSAGE;
                comMes.command[comMes.commandCnt] = '\0';
            } else {
                comMes.command[comMes.commandCnt] = data;
                comMes.commandCnt++;
                if(comMes.commandCnt > 50) {
                    comMes.commandCnt = 0;
                }
            }
            break;
            
        case READ_STATE_MESSAGE:
            if (data == SEP_CHAR) {
                comMes.message[comMes.messageCnt] = '\0';
                readBuffer.comMes[bufferCnt] = comMes;
                register uint8_t i = 0;
                for (i=0; i < comMes.commandCnt; i++) {
                    comMes.command[i] = 0;
                }
                for (i=0; i < comMes.messageCnt; i++) {
                    comMes.message[i] = 0;
                }
                comMes.commandCnt = 0;
                comMes.messageCnt = 0;
                blockLength--;
                bufferCnt++;
                if (blockLength == 0) {
                    readBuffer.state = READ_STATE_ACK_ID;
                } else {
                    readBuffer.state = READ_STATE_COMMAND;
                }
            } else {
                comMes.message[comMes.messageCnt] = data;
                comMes.messageCnt++;
                if(comMes.messageCnt > 50) {
                    comMes.messageCnt = 0;
                }
            }
            break;
            
        case READ_STATE_ACK_ID:
            if (data == SEP_CHAR) {
                readBuffer.state = READ_STATE_END;
            } else {
                readBuffer.ackId = data;
            }
            break;
            
        case READ_STATE_END:
            if (data == STOP_CHAR) {
                acknowledge(readBuffer.ackId);
                readBuffer.state = READ_STATE_START;
                UART_flag = true;
            } else {
                readBuffer.check = data;
            }
            break;
            
        default: 
            readBuffer.state = READ_STATE_START;
            break;
    }
}

void acknowledge(uint8_t ackId) {
    D_UART_WriteByte(START_CHAR);
    D_UART_WriteByte(ACK_CHAR);
    D_UART_WriteByte(ackId);
    D_UART_WriteByte(STOP_CHAR);
    canWrite = true;
}

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/
void C_UART_Init() {
    // Disable first
    canWrite = false;
    
    // UART Driver
    D_UART_Init(UART_MODULE_1, UART1_BAUD);
    D_UART_Enable(UART_MODULE_1, true);
    
    // Clear/set variables
    deviceId = UART1_ID;
    UART_flag = false;
    
    D_INT_EnableUartInterrupts(true);
}

void C_UART_Write(const char* command, const char* data) {
//    if (!canWrite) {
//        return;
//    }
    D_UART_WriteByte(START_CHAR); 
    
    // Id
    D_UART_WriteByte(0x30 + deviceId); 
    // Length
    D_UART_WriteByte(SEP_CHAR); D_UART_WriteByte(0x31); // TODO
    // Command
    D_UART_WriteByte(SEP_CHAR); printf(command);
    // Message
    D_UART_WriteByte(SEP_CHAR); printf(data);
    
    // Acknowledge id
    D_UART_WriteByte(SEP_CHAR); D_UART_WriteByte(0x32); // TODO
    // Check
    D_UART_WriteByte(SEP_CHAR); D_UART_WriteByte(0x33); // TODO
    
    D_UART_WriteByte(STOP_CHAR); 
}

void C_UART_WriteInt(const char* command, int data) {
//    if (!canWrite) {
//        return;
//    }
    D_UART_WriteByte(START_CHAR); 
    
    // Id
    D_UART_WriteByte(0x30 + deviceId); 
    // Length
    D_UART_WriteByte(SEP_CHAR); D_UART_WriteByte(0x31); // TODO
    // Command
    D_UART_WriteByte(SEP_CHAR); printf(command);
    // Message
    D_UART_WriteByte(SEP_CHAR); printf("%d", data);
    
    // Acknowledge id
    D_UART_WriteByte(SEP_CHAR); D_UART_WriteByte(0x32); // TODO
    // Check
    D_UART_WriteByte(SEP_CHAR); D_UART_WriteByte(0x33); // TODO
    
    D_UART_WriteByte(STOP_CHAR); 
}

void C_UART_NewData(uint8_t data) {
    fillDataBuffer(data);
}

READ_Data C_UART_Read(){
    READ_ComMes cm = readBuffer.comMes[0];
    readData.sender = readBuffer.id;
    uint16_t i = 0;
    for (i = 0; i < 3; i++) {
        readData.command[i] = cm.command[i];
    }
    for (i = 0; i < 10; i++) {
        readData.message[i] = cm.message[i];
    }
    return readData;
}

READ_Data C_UART_ReadBlock(uint8_t cnt) {
    READ_ComMes cm = readBuffer.comMes[cnt];
    readData.sender = readBuffer.id;
    uint16_t i = 0;
    for (i = 0; i < 3; i++) {
        readData.command[i] = cm.command[i];
    }
    for (i = 0; i < 10; i++) {
        readData.message[i] = cm.message[i];
    }

    return readData;
}

uint8_t C_UART_BlockLength() {
    return readBuffer.blockLength;
}