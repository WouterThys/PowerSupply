#include "UART_Driver.h"


static readCallBack callback;


void UART_Init(const uint16_t baud, const bool inverted, readCallBack rcb) {
    // Set callback
    callback = rcb;

    // Port settings
    UART_TX_Dir = 1;
    UART_RX_Dir = 1;

    // TXSTA
    TXSTAbits.CSRC = 0;     // Asynchronous mode
    TXSTAbits.TX9 = 0;      // Selects 8-bit transmission
    TXSTAbits.TXEN = 0;     // Keep disabled for now
    TXSTAbits.SYNC = 0;     // Asynchronous mode
    TXSTAbits.BRGH = 1;     // High speed
    
    // RCSTA
    RCSTAbits.RX9 = 0;      // Select 8-bit reception
    RCSTAbits.CREN = 0;     // Keep disabled for now

    // BAUDCON
    BAUDCONbits.RXDTP = (uint8_t) (inverted & 0x01);
    BAUDCONbits.TXCKP = (uint8_t) (inverted & 0x01);
    BAUDCONbits.BRG16 = 0;  // 8-bit Baud Rate Generator

    // Set baud
    uint16_t calc = ((_XTAL_FREQ / baud) / 16) - 1;
    SPBRG = calc;

    // Interrupts
    PIR1bits.TXIF = 0;      // Clear flag
    PIE1bits.TXIE = 0;      // No interrupts for TX 
    PIR1bits.RCIF = 0;      // Clear flag
    PIE1bits.RCIE = 1;      // Enable receive interrupt
    IPR1bits.RCIP = 1;      // High priority    

    // Enable
    TXSTAbits.TXEN = 1;     // Enable transmit
    RCSTAbits.CREN = 1;     // Enable receive
    RCSTAbits.SPEN = 1;         // Enable
}

void putch(unsigned char byte) {
    UART_PutChar(byte);
}


void UART_PutChar(const char data) {
    while(TRMT == 0); // Wait unit buffer is empty
    TXREG = data;
}

void UART_PutString(const char *data, uint8_t length) {
    uint8_t i;
    for (i = 0; i < length; i++) {
        UART_PutChar(*(data + i));
    }
}

void interrupt ISR() {
    if (RCIF) {
        uint8_t data = RCREG; // Read will clear RCIF
        callback(data);
    }
}
