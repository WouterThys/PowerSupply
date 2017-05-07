#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "../Settings.h"
#include "../Drivers/UART_Driver.h"
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
uint8_t baud;
uint8_t deviceId;

const char* startCharacter =       "&";
const char* stopCharacter =        "$";
const char* messageCharacter =     "M";
const char* blockCharacter =       "B";
const char* ackCharacter =         "A";

typedef struct {
    char command[3];      // Command buffer
    uint8_t commandCnt;   // Count of command buffer
    char message[50];  // Message buffer
    uint8_t messageCnt;   // Count of the message buffer
}READ_ComMes;

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

bool D_UART_ReadFlag;

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
                blockLength = 0;
                bufferCnt = 0;
                D_UART_ReadFlag = false;
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
                uint8_t i = 0;
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
                D_UART_ReadFlag = true;
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
    printf(startCharacter);
    
    printf(ackCharacter);
    // id
    printf("%x",ackId);
    
    printf(stopCharacter);
}

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/
void C_UART_Init(uint8_t id, uint16_t baud, bool interrupts) {

    // UART Driver
    D_UART_Init(UART_MODULE_1, baud);
    D_UART_Enable(UART_MODULE_1, true);
    
    // Clear/set variables
    deviceId = id;
    D_UART_ReadFlag = false;
    
    
}

void C_UART_Write(const char* command, const char* data) {
    printf(startCharacter); 
    
    // Id
    printf(":"); printf("%d",deviceId); 
    // Length
    printf(":"); printf("%d",1); // TODO
    // Command
    printf(":"); printf(command);
    // Message
    printf(":"); printf(data);
    
    // Acknowledge id
    printf(":"); printf("%d",2); // TODO
    // Check
    printf(":"); printf("%d",3); // TODO
    
    printf(stopCharacter);
}

void C_UART_WriteInt(const char* command, int data) {
    printf(startCharacter);
    
    printf(messageCharacter);
    printf("%d", deviceId);
    // Command
    printf(":");printf(command);
    // Message
    printf(":");printf("%d",data);
    
    printf(stopCharacter);
}

READ_Data D_UART_Read(){
    READ_ComMes cm = readBuffer.comMes[0];
    readData.sender = readBuffer.id;
    readData.command = cm.command;
    readData.message = cm.message;
    return readData;
}

READ_Data D_UART_ReadBlock(uint8_t cnt) {
    READ_ComMes cm = readBuffer.comMes[cnt];
    readData.sender = readBuffer.id;
    readData.command = cm.command;
    readData.message = cm.message;
    return readData;
}

uint8_t D_UART_BlockLength() {
    return readBuffer.blockLength;
}

//void D_UART_Acknowledge(uint16_t val) {
//    printf(startCharacter);
//    
//    printf(ackCharacter);
//    // id
//    printf("%x",readBuffer.readId);
//    
//    if (val != NULL) {
//        printf(":%d", val);
//    }
//    
//    printf(stopCharacter);
//}


///*******************************************************************************
// *          INTERRUPTS
// ******************************************************************************/
//void interrupt low_priority LowISR(void) {
//    if (PIR1bits.RC1IF) {
//        fillDataBuffer(readByte());
//        PIR1bits.RC1IF = 0;
//    }
//}