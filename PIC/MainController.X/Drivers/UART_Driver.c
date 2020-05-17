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


/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initializeRegisters(uint16_t baud);

/* Event function pointers */
static void (*readDone)(uint8_t data);

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
    U1STAbits.UTXINV = 1; // UxTX Idle state is ?0?*********************************
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

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/
void uartDriverInit(uint16_t baud, void (*onReadDone)(uint8_t data)) {
    
    readDone = onReadDone;
    initializeRegisters(baud);
}

void uartDriverEnable(bool enable) {
    if(enable) {
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
            return;
        } 
        if(U1STAbits.OERR == 1) {
            // Do something with error?
            uartDriverEnable(false);
            DelayUs(10);
            uartDriverEnable(true);
            return;
        } 
       
        readDone(data);
    }
}
