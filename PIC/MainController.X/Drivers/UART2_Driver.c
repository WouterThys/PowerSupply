#include "UART2_Driver.h"


/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize2Registers(uint16_t baud);

/* Event function pointers */
static void (*readDone2)(uint8_t data);

static void initialize2Registers(uint16_t baud) {

    // U1MODE register settings
    U2MODEbits.UARTEN = 0; // UARTx is disabled; UARTx pins are controlled by the corresponding PORTx, LATx and TRISx bits
    U2MODEbits.USIDL = 0; // Continues operation in Idle mode 
    U2MODEbits.IREN = 0; // IrDA encoder and decoder are disabled
    U2MODEbits.UEN = 0b00; // UxTX and UxRX pins are enabled and used; UxCTS, UxRTS and BCLKx pins are controlled by port latches        
    U2MODEbits.WAKE = 0; // Wake-up is disabled
    U2MODEbits.LPBACK = 0; // Loop-back mode is disabled
    U2MODEbits.ABAUD = 0; // Baud rate measurement is disabled or complete
    U2MODEbits.URXINV = 0; // UxRX Idle state is ?0?***************************
    U2MODEbits.BRGH = 0; // BRG generates 16 clocks per bit period (16x baud clock, Standard Speed mode)
    U2MODEbits.PDSEL = 0b00; // 8-bit data, no parity
    U2MODEbits.STSEL = 0; // 1 Stop bit
    
    // U1STA register settings
    U2STAbits.UTXISEL0 = 0; // Interrupt is generated when any character is transferred to the Transmit Shift Register and the
    U2STAbits.UTXISEL1 = 0; // transmit buffer is empty (which implies at least one location is empty in the transmit buffer) 
    U2STAbits.UTXINV = 0; // UxTX Idle state is ?0?*********************************
    U2STAbits.UTXEN = 0; // UARTx transmitter is disabled; any pending transmission is aborted and the buffer is reset, UxTX pin is controlled by the port
    U2STAbits.URXISEL = 0b00; // Interrupt flag bit is set when a character is received
    U2STAbits.ADDEN = 0; // Address Detect mode is disabled
    
    // U1BRG register settings
    U2BRG = ((FCY/baud) >> 4) - 1;
    
    // Clear buffers
    while (U2STAbits.URXDA) {
        uint8_t dummy = U2RXREG;
        dummy++;
    }
    U2TXREG = 0x0000;
    
    _U2RXIF = 0; // Clear flag
    _U2TXIF = 0;
    _U2RXIP = IP_U2RX; // Priority
    _U2RXIE = 1; // Enable interrupts
    
    
    // Port initializes
    UART2_RX_Dir = 1;
    UART2_TX_Dir = 0;
    
    // Mapping
    RPINR19bits.U2RXR = UART2_RX_Map;
    UART2_TX_Map = 0b000011;
}

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/
void uart2DriverInit(uint16_t baud, void (*onReadDone)(uint8_t data)) {
    
    readDone2 = onReadDone;
    initialize2Registers(baud);
}

void uart2DriverEnable(bool enable) {
    if(enable) {
        U2MODEbits.UARTEN = 1; // UARTx is enabled
        U2STAbits.UTXEN = 1; 
    } else {
        U2MODEbits.UARTEN = 0; // UARTx is disabled
        U2STAbits.UTXEN = 0; 
    }
}

void uart2DriverWrite(uint8_t data) {
    U2TXREG = data;
    while(U2STAbits.UTXBF == 1);
    DelayUs(1);
}

void putch(char data) {
    uart2DriverWrite(data);
}

// UART Received byte
void __attribute__ ( (interrupt, no_auto_psv) ) _U2RXInterrupt(void) {
    if (_U2RXIF) {
        
        uint8_t data = U2RXREG;
        _U2RXIF = 0; // Clear interrupt
        
        if(U2STAbits.FERR == 1) {
            // Do something with error?
            return;
        } 
        if(U2STAbits.OERR == 1) {
            // Do something with error?
            uart2DriverEnable(false);
            DelayUs(10);
            uart2DriverEnable(true);
            return;
        } 
       
        readDone2(data);     
    }
}
