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

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initUart_1(uint16_t baud);
static void initUart_2(uint16_t baud);

static void setUartPorts(uint16_t which);

void initUart_1(uint16_t baud) {
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
    U1BRG = ((FCY/baud) >> 4);
    
    // Clear buffers
    U1TXREG = 0x0000;
    U1RXREG = 0x0000;
    
    D_UART_Enable(UART_MODULE_1, true);
}

void initUart_2(uint16_t baud) {
    
}

void setUartPorts(uint16_t which){
    if(which == UART_MODULE_1) {
        // Tri-states
        UART1_RX_Dir = 1;
        UART1_TX_Dir = 0;
    
        // Mapping
        RPINR18bits.U1RXR = UART1_RX;
        RPOR2bits.RP38R = UART1_TX;
    } else if(which == UART_MODULE_2) {
        //
    } else {
        return;
    }
}

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/
void D_UART_Init(uint16_t which, uint16_t baud) {
    // Register initializes
    if(which == UART_MODULE_1) {
        initUart_1(baud);
    } else if (which == UART_MODULE_2) {
        initUart_2(baud);
    } else {
        return;
    }
    
    // Port initializes
    setUartPorts(which);
}

void D_UART_Enable(uint16_t which, bool enable) {
    if (which == UART_MODULE_1) {
        if(enable) {
            U1MODEbits.UARTEN = 1; // UARTx is enabled
            U1STAbits.UTXEN = 1; 
        } else {
            U1MODEbits.UARTEN = 0; // UARTx is disabled
            U1STAbits.UTXEN = 0; 
        }
    } else {
        if(enable) {
            U2MODEbits.UARTEN = 1; // UARTx is enabled
            U2STAbits.UTXEN = 1; 
        } else {
            U2MODEbits.UARTEN = 0; // UARTx is disabled
            U2STAbits.UTXEN = 0; 
        }
    }
}

void D_UART_WriteByte(uint8_t data) {
    U1TXREG = data;
    while(U1STAbits.TRMT == 0);
}

uint8_t D_UART_ReadByte() {
    if(U1STAbits.FERR == 1) {
        uint8_t dum = U1RXREG;
        return dum*0;
    } 
    if(U1STAbits.OERR == 1) {
        D_UART_Enable(UART_MODULE_1, false);
        DelayUs(10);
        D_UART_Enable(UART_MODULE_1, true);
        return 0;
    } 
    return U1RXREG;
}

void putch(char data) {
    D_UART_WriteByte(data);
}