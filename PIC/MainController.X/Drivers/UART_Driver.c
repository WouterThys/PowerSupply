#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "../Settings.h"
#include "SYSTEM_Driver.h"
#include "UART_Driver.h"
/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
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
static UartData_t readData;


/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initializeRegisters(uint16_t baud);
static void convertMessage(uint8_t * data, uint8_t length);
static void fillDataBuffer(uint8_t data);

/* Event function pointers */
static void (*readDone)(UartData_t data);

/* Acknowledge ? */
#ifdef ACKNOWLEDGE
static void acknowledge(uint8_t ackId);
#endif

void initializeRegisters(uint16_t baud) {

    // U1MODE register settings
    U1MODEbits.UARTEN = 0; // UARTx is disabled; UARTx pins are controlled by the corresponding PORTx, LATx and TRISx bits
    U1MODEbits.USIDL = 0; // Continues operation in Idle mode 
    U1MODEbits.IREN = 0; // IrDA encoder and decoder are disabled
    U1MODEbits.UEN = 0b00; // UxTX and UxRX pins are enabled and used; UxCTS, UxRTS and BCLKx pins are controlled by port latches        
    U1MODEbits.WAKE = 0; // Wake-up is disabled
    U1MODEbits.LPBACK = 0; // Loop-back mode is disabled
    U1MODEbits.ABAUD = 0; // Baud rate measurement is disabled or complete
    U1MODEbits.URXINV = 0; // UxRX Idle state is ?0?***************************
    U1MODEbits.BRGH = 0; // BRG generates 16 clocks per bit period (16x baud clock, Standard Speed mode)
    U1MODEbits.PDSEL = 0b00; // 8-bit data, no parity
    U1MODEbits.STSEL = 0; // 1 Stop bit
    
    // U1STA register settings
    U1STAbits.UTXISEL0 = 0; // Interrupt is generated when any character is transferred to the Transmit Shift Register and the
    U1STAbits.UTXISEL1 = 0; // transmit buffer is empty (which implies at least one location is empty in the transmit buffer) 
    U1STAbits.UTXINV = 0; // UxTX Idle state is ?0?*********************************
    U1STAbits.UTXEN = 0; // UARTx transmitter is disabled; any pending transmission is aborted and the buffer is reset, UxTX pin is controlled by the port
    U1STAbits.URXISEL = 0b00; // Interrupt flag bit is set when a character is received
    U1STAbits.ADDEN = 0; // Address Detect mode is disabled
    
    // U1BRG register settings
    U1BRG = ((FCY/baud) >> 4) - 1;
    
    // Clear buffers
    while (U1STAbits.URXDA) {
        uint8_t dummy = U1RXREG;
        dummy++;
    }
    U1TXREG = 0x0000;
    
    _U1RXIF = 0; // Clear flag
    _U1RXIP = IP_U1RX; // Priority
    _U1RXIE = 1; // Enable interrupts
    
    
    // Port initializes
    UART1_RX_Dir = 1;
    UART1_TX_Dir = 0;
    
    // Mapping
    RPINR18bits.U1RXR = UART1_RX_Map;
    UART1_TX_Map = 0b000001;
}



#ifdef TYPE_TEXT
void convertMessage(uint8_t * data, uint8_t length) {
    uint8_t i = 0;
    while (i < length) {
        readData.message[i] = *(data + i);
        i++;
    }
    readData.message[i] = '\0'; // Close string
    readData.messageLength = i+1;
    
    (*readDone)(readData);
}
#endif

#ifdef TYPE_SIMPLE_SHORT
// $C:M&
#define LENGTH  5           /* Lenth of the total message                     */
#define START   0           /* Position of start character                    */
#define STOP    4           /* Position of stop character                     */
#define SEP     2           /* Position of separator character                */
#define COM     START + 1   /* First position of command                      */
#define MES     STOP  - 1   /* First position of message                      */
void convertMessage(uint8_t * data, uint8_t length) {
    uint8_t e = STA_OK; // Error
    if (length != LENGTH) e = STA_INVALID_LENGTH;
    if (data[START] != START_CHAR) e |= STA_INVALID_START;
    if (data[STOP] != STOP_CHAR) e |= STA_INVALID_STOP;
    if (data[SEP] != SEP_CHAR) e |= STA_INVALID_SEP;
    
    if (e == STA_OK) {
        readData.command = data[COM];
        readData.message[0] = data[MES];
    }
    
    readData.status = e;
    readData.messageLength = 1;
    
    (*readDone)(readData);
}
#endif

#ifdef TYPE_SIMPLE_LONG
// $C:MM:A&
#define LENGTH  8           /* Length of the total message                    */
#define START   0           /* Position of start character                    */
#define STOP    LENGTH -1   /* Position of stop character                     */
#define SEP1    2           /* Position of separator 1 character              */
#define SEP2    5           /* Position of separator 2 character              */
#define COM     START + 1   /* First position of command                      */
#define MES     SEP1  + 1   /* First position of message                      */
#define ACK     STOP  - 1   /* Position of acknowledge character              */
void convertMessage(uint8_t * data, uint8_t length) {
    uint8_t e = STA_OK; // Error
    
    if (length != LENGTH) e = STA_INVALID_LENGTH;
    if (data[START] != START_CHAR) e |= STA_INVALID_START;
    if (data[STOP] != STOP_CHAR) e |= STA_INVALID_STOP;
    if (data[SEP1] != SEP_CHAR) e |= STA_INVALID_SEP;
    if (data[SEP2] != SEP_CHAR) e |= STA_INVALID_SEP;
    
    if (e == STA_OK) {
        readData.command = data[COM];
        
        readData.message[0] = data[MES];
        readData.message[1] = data[MES+1];
    }
#ifdef ACKNOWLEDGE      
    if (e == STA_OK) {
        readData.ack = data[ACK];
        acknowledge(readData.ack);
    }
#endif
    
    readData.status = e;
    readData.messageLength = 2;
    
    (*readDone)(readData);
}
#endif

#ifdef TYPE_VARIABLE
// $C:L:MM...ML:A&
#define LENGTH  8           /* Length of total message without message bytes  */
#define START   0           /* Position of start character                    */
#define SEP1    2           /* Position of separator 1 character              */
#define SEP2    4           /* Position of separator 2 character              */
#define LEN     3           /* Position of length parameter                   */
#define COM     START + 1   /* First position of command                      */
#define MES     SEP2  + 1   /* First position of message                      */
void convertMessage(uint8_t * data, uint8_t length) {
    uint8_t e = STA_OK; // Error
    if (data[START] != START_CHAR) e |= STA_INVALID_START;
    if (data[length - 1] != STOP_CHAR) e |= STA_INVALID_STOP;
    if (data[SEP1] != SEP_CHAR) e |= STA_INVALID_SEP;
    if (data[SEP2] != SEP_CHAR) e |= STA_INVALID_SEP;
    if (data[length - 3] != SEP_CHAR) e |= STA_INVALID_SEP;
    
    if (e == STA_OK) {
        uint8_t l = data[LEN];
        
        if ((l + LENGTH) == length) {
            readData.command = data[COM];
            readData.messageLength = l;

            uint8_t cnt = 0;
            while(cnt < l) {
                readData.message[cnt] = data[MES + cnt];
                cnt++;
            }
        } else {
            e |= STA_INVALID_LENGTH;
        }
    }
#ifdef ACKNOWLEDGE      
    if (e == STA_OK) {
        readData.ack = data[length - 2];
        acknowledge(readData.ack);
    }
#endif
   
    readData.status = e;
    (*readDone)(readData);
}
#endif



#ifdef ACKNOWLEDGE
void acknowledge(uint8_t ackId) {
    uartDriverWriteByte(START_CHAR);
    uartDriverWriteByte(ACK_CHAR);
    uartDriverWriteByte(ackId);
    uartDriverWriteByte(STOP_CHAR);
}
#endif

#define S0  0 /* State idle */
#define S1  1 /* State */
#define S2  2 /* State */

static uint8_t inputBufferCnt = 0;
static uint8_t inputBuffer[100];
static uint8_t state;
void fillDataBuffer(uint8_t data) {
    // Check
    if (inputBufferCnt > 100) {
        inputBufferCnt = 0;
        return;
    }
     
    // FSM
    switch (state) {
        default:
        case S0:
            if (data == START_CHAR) {
                inputBuffer[0] = START_CHAR;
                inputBufferCnt = 1;
                state = S1;
            }
            break;

        case S1:
            if (data == STOP_CHAR) {
                state = S2;
            } 
            inputBuffer[inputBufferCnt] = data;
            inputBufferCnt++;
            break;

        case S2:
            if (data == '\0') {
                convertMessage(inputBuffer, inputBufferCnt);
            }
            state = S0;
            break;
    }
}

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/
void uartDriverInit(uint16_t baud, void (*onReadDone)(UartData_t data)) {
    
    readDone = onReadDone;
    initializeRegisters(baud);
}

void uartDriverEnable(bool enable) {
    if(enable) {
        inputBufferCnt = 0;
        U1MODEbits.UARTEN = 1; // UARTx is enabled
        U1STAbits.UTXEN = 1; 
    } else {
        U1MODEbits.UARTEN = 0; // UARTx is disabled
        U1STAbits.UTXEN = 0; 
    }
}

void uartDriverWriteByte(uint8_t data) {
    U1TXREG = data;
    while(U1STAbits.TRMT == 0);
    if (DEBUG) {
        DelayUs(1000);
    } else {
        DelayUs(1);
    }
}

void uartDriverWriteString(const char * str, uint8_t length) {
    uint8_t i = 0;
    for (i = 0; i < length; i++) {
        uartDriverWriteByte(*(str + i));
    }
    uartDriverWriteByte(0); // Write 0 terminator
}

uint8_t uartReadByte() {
    return U1RXREG;
}

void putch(char data) {
    uartDriverWriteByte(data);
}


void uartDriverWrite(char command, char* data) {
    uartDriverWriteByte(START_CHAR); 
    uartDriverWriteByte(command);
    uartDriverWriteByte(SEP_CHAR);
    
#ifdef TYPE_SIMPLE_SHORT
    uartDriverWriteByte(*data);
#endif
#ifdef TYPE_SIMPLE_LONG
    uartDriverWriteByte(*data);
    uartDriverWriteByte(*(data+1));
#endif
#ifdef TYPE_VARIABLE
#endif
    uartDriverWriteByte(STOP_CHAR); 
}

void uartDriverWriteInt(char command, int data) {
//    if (!canWrite) {
//        return;
//    }
    uartDriverWriteByte(START_CHAR); 
    
    // Id
    uartDriverWriteByte(0x30 + deviceId); 
    // Length
    uartDriverWriteByte(SEP_CHAR); uartDriverWriteByte(0x31); // TODO
    // Command
    uartDriverWriteByte(SEP_CHAR); printf("%d", command);
    // Message
    uartDriverWriteByte(SEP_CHAR); printf("%d", data);
    
    // Acknowledge id
    uartDriverWriteByte(SEP_CHAR); uartDriverWriteByte(0x32); // TODO
    // Check
    uartDriverWriteByte(SEP_CHAR); uartDriverWriteByte(0x33); // TODO
    
    uartDriverWriteByte(STOP_CHAR); 
}




// UART Received byte
void __attribute__ ( (interrupt, no_auto_psv) ) _U1RXInterrupt(void) {
    if (_U1RXIF) {
        
        uint8_t data = U1RXREG;
        _U1RXIF = 0; // Clear interrupt
        
        if(U1STAbits.FERR == 1) {
            // Do something with error?
            inputBufferCnt = 0;
            return;
        } 
        if(U1STAbits.OERR == 1) {
            // Do something with error?
            uartDriverEnable(false);
            DelayUs(10);
            uartDriverEnable(true);
            inputBufferCnt = 0;
            return;
        } 
       
        fillDataBuffer(data);
    }
}
