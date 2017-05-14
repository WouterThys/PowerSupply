#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <math.h>
#include <stdlib.h>          /* Includes math definitions                       */

#include "../Settings.h"
#include "../Headers/custom_char.h"

#include "SYSTEM_Driver.h"
#include "LCD_Driver.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
#define BUSY_FLAG_MASK  0x80    /* Mask to read the busy flag. 1 = Busy       */
#define ADDR_MASK       0x3F    /* Mask to read adres while reading Busy Flag */

#define _CLEAR_DISPLAY  0x01    /* Clear entire display and sets DDRAM address 0 in address counter */
#define _RETURN_HOME    0x02    /* Sets DDRAM address 0 in address counter.   */
#define _ENTRY_MODE_SET 0x04    /* Sets cursor move direction and specifies display shift. Add ID and S options */
#define _ONOFF_CONTROL  0x08    /* Sets entire display on/off, cursor on/off, blinking on/off */
#define _CD_SHIFT       0x10    /* Moves cursor and shifts display without changing DDRAM contents */
#define _FUNCTION_SET   0x20    /* Sets the interface data length, number of display lines and character font */
#define _DDRAM_ADDR     0x80    /* Sets the DDRAM address                     */

#define ID_OFF          0x00    /* Don't increment. OR with _ENTRY_MODE_SET   */
#define ID_ON           0x02    /* Increment by 1. OR with _ENTRY_MODE_SET    */
#define SHIFT_OFF       0x00    /* No shift. OR with _ENTRY_MODE_SET          */
#define SHIFT_ON        0x01    /* Shift. OR with _ENTRY_MODE_SET             */
#define EIGHTBIT        0x10    /* 8-bit interface data. OR with _FUNCTION_SET*/
#define FOURBIT         0x00    /* 4-bit interface data. OR with _FUNCTION_SET*/
#define ONELINE         0x00    /* 1-line display. OR with _FUNCTION_SET      */
#define TWOLINE         0x08    /* 2-line display. OR with _FUNCTION_SET      */
#define DOT5x8          0x00    /* 5x8 dot character. OR with _FUNCTION_SET   */
#define DOT5x7          0x04    /* 5x7 dot character. OR with _FUNCTION_SET   */
#define DISPLAY_ON      0x04    /* Display on. OR with _ONOFF_CONTROL         */
#define DISPLAY_OFF     0x00    /* Distplay off. OR with _ONOFF_CONTROL       */
#define CURSOR_ON       0x02    /* Cursor on.  OR with _ONOFF_CONTROL         */
#define CURSOR_OFF      0x00    /* Cursor off.  OR with _ONOFF_CONTROL        */
#define BLINK_ON        0x01    /* Blink on.  OR with _ONOFF_CONTROL          */
#define BLINK_OFF       0x00    /* Blink off.  OR with _ONOFF_CONTROL         */


/*******************************************************************************
 *          LOCAL FUNCTION DEFINES
 ******************************************************************************/
static void lcdWriteToPort(uint8_t data);
static void lcdSendCommand(uint8_t cmd);
static void lcdSendData(uint8_t data);

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/
#define lcdSetE() (LCD_E=1)
#define lcdSetRS() (LCD_RS=1)
#define lcdSetRW() (LCD_RW=1)

#define lcdClrE() (LCD_E=0)
#define lcdClrRS() (LCD_RS=0)
#define lcdClrRW() (LCD_RW=0)

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/

static uint8_t D;  /* Display on or off                                       */
static uint8_t C;  /* Cursor on or off                                        */
static uint8_t B;  /* Cursor blink on or off                                  */

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void lcdWriteToPort(uint8_t data) {
    D0 = (data >> 0) & 0x01;
    D1 = (data >> 1) & 0x01;
    D2 = (data >> 2) & 0x01;
    D3 = (data >> 3) & 0x01;
    D4 = (data >> 4) & 0x01;
    D5 = (data >> 5) & 0x01;
    D6 = (data >> 6) & 0x01;
    D7 = (data >> 7) & 0x01;
}

void lcdSendCommand(uint8_t cmd) {
    lcdClrRS();
    lcdWriteToPort(cmd);
    
    lcdSetE();
    DelayUs(50);
    lcdClrE();
    DelayMs(100);
}

void lcdSendData(uint8_t data) {
    lcdSetRS();
    lcdWriteToPort(data);
    
    lcdSetE();
    DelayUs(50);
    lcdClrE();
    DelayUs(50);
}


/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/

void D_LCD_Init() {

    TRISB = 0;
    TRISA = 0;
    
    // Start up
    lcdClrE();
    DelayMs(100);
    lcdSendCommand(0x30); 
    DelayMs(30);
    lcdSendCommand(0x30); 
    DelayMs(10);
    lcdSendCommand(0x30); 
    DelayMs(10);
    
    // Set to default values
    lcdSendCommand(_CLEAR_DISPLAY); // Clear display 
    lcdSendCommand(_RETURN_HOME); // Return home 
    lcdSendCommand(_FUNCTION_SET + (EIGHTBIT | TWOLINE | DOT5x8)); 
    lcdSendCommand(_ENTRY_MODE_SET + (ID_ON | SHIFT_OFF)); 
    lcdSendCommand(_ONOFF_CONTROL + (DISPLAY_OFF | CURSOR_OFF | BLINK_OFF)); 
    
    D = DISPLAY_OFF;  // Display is off
    C = CURSOR_OFF;  // Cursor is off
    B = BLINK_OFF;  // Blink is off 
}

void D_LCD_Enable(bool enable) {
    if (enable) {
        D = DISPLAY_ON;
        lcdSendCommand(_ONOFF_CONTROL + (D | C | B)); 
    } else {
        D = DISPLAY_OFF;
        lcdSendCommand(_ONOFF_CONTROL + (D | C | B)); 
    }
}

void D_LCD_ClearSreen() {
    lcdSendCommand(_CLEAR_DISPLAY);
    lcdSendCommand(_RETURN_HOME);
}

void D_LCD_Goto(uint8_t line, uint8_t pos) {
    switch(line) {
        case 1:
            if (pos < 16) {
                lcdSendCommand(_DDRAM_ADDR + pos);
            }
            break;
        case 2:
            if (pos < 16) {
                lcdSendCommand(_DDRAM_ADDR + (pos + 0x40));
            }
            break;
        default:
            break;
    }
}

void D_LCD_CursorStyle(bool on, bool blink) {
    if (on) {
        C = CURSOR_ON;
    } else {
        C = CURSOR_OFF;
    }
    if (blink) {
        B = BLINK_ON;
    } else {
        B = BLINK_OFF;
    }
    lcdSendCommand(_ONOFF_CONTROL + (D | C | B));
}

void D_LCD_WriteString(char * str) {
    while (str && *str) {
        lcdSendData(*str++);
    }
}

void D_LCD_WriteInt(int16_t value) {
    uint8_t digits[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // 16 digits is LCD max
    
    if (value < 0) {
        lcdSendData(0x2D); // Minus sign
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
            lcdSendData(0x30 + digits[i]); // Add 0x30 to get ASCII character
        }
    } else {
        lcdSendData(0x30);
    }
}

void D_LCD_WriteDouble(double value, uint16_t precision) {
    D_LCD_WriteInt((int16_t)value); // Write digits before decimal point
    D_LCD_WriteString(",");

    if (value < 0) {
        value = value * (-1);
    }
    double fraction = value - (uint16_t) value;
    
    while (precision) {
        fraction = fraction * 10;

        D_LCD_WriteInt((uint16_t)fraction);
        fraction = fraction - (uint16_t) fraction;
        
        precision--;
    }
}
