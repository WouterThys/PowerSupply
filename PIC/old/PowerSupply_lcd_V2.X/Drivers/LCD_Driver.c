#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <string.h>        /* Includes string functions                       */

#include "../Settings.h"
#include "SYSTEM_Driver.h"
#include "SPI_Driver.h"
#include "LCD_Driver.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
#define LCD_PREFIX      0xFE
    
#define LCD_DISPLAY_ON           0x41
#define LCD_DISPLAY_OFF          0x42
#define LCD_SET_CURSOR           0x45
#define LCD_CURSOR_HOME          0x46
#define LCD_UNDERLINE_CURSOR_ON  0x47
#define LCD_UNDERLINE_CURSOR_OFF 0x48
#define LCD_MOVE_CURSOR_LEFT     0x49
#define LCD_MOVE_CURSOR_RIGHT    0x4A
#define LCD_BLINKING_CURSOR_ON   0x4B
#define LCD_BLINKING_CURSOR_OFF  0x4C
#define LCD_BACKSPACE            0x4E
#define LCD_CLEAR_SCREEN         0x51
#define LCD_SET_CONTRAST         0x52
#define LCD_SET_BRIGHTNESS       0x53
#define LCD_LOAD_CUSTOM_CHAR     0x54
#define LCD_MOVE_DISPLAY_LEFT    0x55
#define LCD_MOVE_DISPLAY_RIGHT   0x56
#define LCD_CHANGE_RS232_BAUD    0x61
#define LCD_CHANGE_I2C_ADDRESS   0x62
#define LCD_DISPLAY_FIRMWARE     0x70
#define LCD_DISPLAY_RS232_BAUD   0x71
#define LCD_DISPLAY_I2C_ADDRESS  0x72

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/
#define lcdWrite(w)      LCD_SS_Pin = 0; D_SPI_Write(w); LCD_SS_Pin = 1


/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/

/*******************************************************************************
 *          BASIC FUNCTIONS
 ******************************************************************************/


/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/
void lcdInit() {
    D_SPI_Init();
    D_SPI_Enable(true);
    
    lcdTurnDisplayOn(true);
    lcdClearScreen();
}

void lcdWriteChar(char c) {
    lcdWrite(c);
    DelayUs(100);
}

void lcdWriteDigit(uint16_t d) {
    lcdWrite(d + 0x30);
    DelayUs(100);
}

void lcdWriteString(const char *str) {
    uint16_t c = 0;
    while (c < strlen(str)) {
        lcdWriteChar(*(str + c));
        c++;
    }
}

void lcdWriteInt(int16_t value) {
    uint8_t digits[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // 16 digits is LCD max
    
    if (value < 0) {
        lcdWriteChar(0x2D); // Minus sign
        value = value * (-1);
    }
    
    if (value > 0) {
        uint16_t count = 0;
        while (value) {
            digits[count] = value % 10;
            value = value / 10;
            count++;
        }
        int16_t i = 0;
        for (i = count-1; i >= 0; i--) {
            lcdWriteChar(0x30 + digits[i]); // Add 0x30 to get ASCII character
        }
    } else {
        lcdWriteChar(0x30);
    }
}

void lcdWriteDouble(double value, uint16_t precision) {
    lcdWriteInt((int16_t)value); // Write digits before decimal point
    if (precision > 0) {
        lcdWriteString(",");

        if (value < 0) {
            value = value * (-1);
        }
        double fraction = value - (uint16_t) value;

        while (precision) {
            fraction = fraction * 10;

            lcdWriteInt((uint16_t)fraction);
            fraction = fraction - (uint16_t) fraction;

            precision--;
        }
    }
}

void lcdChangeI2CAddress(uint8_t address) {
    lcdWrite(LCD_PREFIX);
    lcdWrite(LCD_CHANGE_I2C_ADDRESS);
    lcdWrite(address);
    DelayUs(3000);
}

void lcdChangeBaudRate(uint8_t baud) {
    lcdWrite(LCD_PREFIX);
    lcdWrite(LCD_CHANGE_RS232_BAUD);
    lcdWrite(baud);
    DelayUs(3000);
}

void lcdTurnDisplayOn(bool on) {
    lcdWrite(LCD_PREFIX);
    if (on) {
        lcdWrite(LCD_DISPLAY_ON);
    } else {
        lcdWrite(LCD_DISPLAY_OFF);
    }
    DelayUs(100);
}

void lcdSetCursorPosition(uint8_t line, uint8_t x) {
    if (line > 1) line = 1;
    if (x > 0x0F) x = 0x0F;
    
    uint8_t pos = x;
    if (line == 1) {
        pos += 0x40;
    }
    
    lcdWrite(LCD_PREFIX);
    lcdWrite(LCD_SET_CURSOR);
    lcdWrite(pos);
    DelayUs(100);
}

void lcdSetCursorHome() {
    lcdWrite(LCD_PREFIX);
    lcdWrite(LCD_CURSOR_HOME);
    DelayUs(1500);
}

void lcdCursorUnderlineOn(bool on) {
    lcdWrite(LCD_PREFIX);
    if (on) {
        lcdWrite(LCD_UNDERLINE_CURSOR_ON);
    } else {
        lcdWrite(LCD_UNDERLINE_CURSOR_OFF);
    }
    DelayUs(1500);
}

void lcdMoveCursorLeft() {
    lcdWrite(LCD_PREFIX);
    lcdWrite(LCD_MOVE_CURSOR_LEFT);
    DelayUs(100);
}

void lcdMoveCursorRight() {
    lcdWrite(LCD_PREFIX);
    lcdWrite(LCD_MOVE_CURSOR_RIGHT);
    DelayUs(100);
}

void lcdTurnOnBlinkingCursor(bool on) {
    lcdWrite(LCD_PREFIX);
    if (on) {
        lcdWrite(LCD_BLINKING_CURSOR_ON);
    } else {
        lcdWrite(LCD_BLINKING_CURSOR_OFF);
    }
    DelayUs(100);
}

void lcdBackspace() {
    lcdWrite(LCD_PREFIX);
    lcdWrite(LCD_BACKSPACE);
    DelayUs(100);
}

void lcdClearScreen() {
    lcdWrite(LCD_PREFIX);
    lcdWrite(LCD_CLEAR_SCREEN);
    DelayUs(1500);
}

void lcdSetDisplayContrast(uint8_t contrast) {
    if (contrast > 50) contrast = 50;
    if (contrast < 1 ) contrast = 1;
    lcdWrite(LCD_PREFIX);
    lcdWrite(LCD_SET_CONTRAST);
    lcdWrite(contrast);
    DelayUs(500);
}

void lcdSetDisplayBrightness(uint8_t brightness) {
    if (brightness > 8) brightness = 8;
    if (brightness < 1) brightness = 1;
    lcdWrite(LCD_PREFIX);
    lcdWrite(LCD_SET_BRIGHTNESS);
    lcdWrite(brightness);
    DelayUs(100);
}

void lcdLoadCustomChar(uint8_t address, const char * character) {
    lcdWrite(LCD_PREFIX);
    lcdWrite(LCD_LOAD_CUSTOM_CHAR);
    lcdWrite(address);
    uint8_t c;
    for (c = 0; c < 8; c++) {
        lcdWrite(*(character + c));
    }
    DelayUs(200);
}

void lcdShiftDisplayLeft() {
    lcdWrite(LCD_PREFIX);
    lcdWrite(LCD_MOVE_DISPLAY_LEFT);
    DelayUs(100);
}

void lcdShiftDisplayRight() {
    lcdWrite(LCD_PREFIX);
    lcdWrite(LCD_MOVE_DISPLAY_RIGHT);
    DelayUs(100);
}

void lcdDisplayFirmwareVersion() {
    lcdWrite(LCD_PREFIX);
    lcdWrite(LCD_DISPLAY_FIRMWARE);
    DelayUs(4000);
}

void lcdDisplayBaudRate() {
    lcdWrite(LCD_PREFIX);
    lcdWrite(LCD_DISPLAY_RS232_BAUD);
    DelayUs(10000);
}

void lcdDisplayI2CAddress() {
    lcdWrite(LCD_PREFIX);
    lcdWrite(LCD_DISPLAY_I2C_ADDRESS);
    DelayUs(4000);
}


