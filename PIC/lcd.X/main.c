/*
 * File:   main.c
 * Author: wouter
 *
 * Created on October 22, 2016, 5:17 PM
 */
#include <xc.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define _XTAL_FREQ 48000000

#define LCD_E      PORTCbits.RC0
#define LCD_RS     PORTCbits.RC1
#define LCD_RW     PORTCbits.RC2
#define LCD_DATA_W PORTB

#define lcdSetE() (LCD_E=1)
#define lcdSetRS() (LCD_RS=1)
#define lcdSetRW() (LCD_RW=1)

#define lcdClrE() (LCD_E=0)
#define lcdClrRS() (LCD_RS=0)
#define lcdClrRW() (LCD_RW=0)

void initialize();
void lcdDataWrite(void);
void lcdDataRead(void);
void lcdCheckBusy(void);
void lcdSendCommand(uint8_t cmd);
void lcdSendData(uint8_t data);
void lcdSendString(char *s);

void initialize() {
    TRISA = 0x00;   PORTA = 0x00;
    TRISB = 0x00;   PORTB = 0x00;
    TRISC = 0x00;   PORTC = 0x00;
    ADCON1 = 0x0F; 
    
    lcdSendCommand(0x38); // 8 bit mode
    lcdSendCommand(0x0E); // Clear the screen
    lcdSendCommand(0x01); // Display cursor on
    lcdSendCommand(0x06); // Increment cursor
    lcdSendCommand(0x80); // Cursor position
}

void lcdDataWrite(void) {
    lcdSetE();
    __delay_us(2);
    lcdClrE();
}

void lcdDataRead(void) {
    lcdClrE();
    __delay_us(2);
    lcdSetE();
}

void lcdCheckBusy(void) {
    TRISBbits.TRISB7 = 1; // Busy flag input

    while (PORTBbits.RB7 == 1) {
        lcdDataRead();
    }

    TRISBbits.TRISB7 = 0; // Output again
}

void lcdSendCommand(uint8_t cmd) {
    lcdCheckBusy();
    lcdClrRW();
    lcdClrRS();
    
    LCD_DATA_W = cmd;
    lcdDataWrite();
}

void lcdSendData(uint8_t data) {
    lcdCheckBusy();
    lcdClrRW();
    lcdSetRS();
    
    LCD_DATA_W = data;
    lcdDataWrite();
}

void lcdSendString(char *s) {
    while (*s) {
        lcdSendData(*s);
        s++;
    }
}

void main(void) {
    
    initialize();
    
    lcdSendString("panda");
    
    while(1) {
        
    }
    return;
}