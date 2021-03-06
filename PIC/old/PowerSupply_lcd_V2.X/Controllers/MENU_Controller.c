#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "MENU_Controller.h"
#include "SUPPLIES_Controller.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/

typedef enum {
    INITIAL,
    MEASURE_VALUES,
    SELECT_VOLTAGE,
    SELECT_CURRENT,
    CHANGE_VOLTAGE,
    CHANGE_CURRENT
} Menu_e;

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/
#define clearScreen() lcdClearScreen()
#define writeString(x,y,str) lcdSetCursorPosition(x,y); lcdWriteString(str) 
#define writeChar(x,y,c) lcdSetCursorPosition(x,y); lcdWriteChar(c)
#define writeDigit(x,y,d) lcdSetCursorPosition(x,y); lcdWriteDigit(d)

/*******************************************************************************
 *          LOCAL FUNCTION DEFINES
 ******************************************************************************/
static void drawValue(uint8_t line, uint8_t startPos, uint16_t value);
/* Event function pointers */
static bool (*putCommand)(Command_t data);

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static Menu_e currentMenu;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
// TODO put on top
// TODO more edit
void drawValue(uint8_t line, uint8_t startPos, uint16_t value) {
    uint16_t v = value / 10;
    uint8_t p = startPos;
    
    uint16_t d3 = v % 10;
    v /= 10;
    uint16_t d2 = v % 10;
    v /= 10;
    uint16_t d1 = v % 10;
    v /= 10;
    uint16_t d0 = v % 10;
    
    writeDigit(line, p++, d0);
    writeDigit(line, p++, d1);
    writeChar(line, p++, ',');
    writeDigit(line, p++, d2);
    writeDigit(line, p, d3);
    
}

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/

void menuInit(bool (*onPutCommand)(Command_t data)) {
    lcdInit();
    
    // Event
    putCommand = onPutCommand;
    
    // Initial value
    currentMenu = INITIAL;
    menuUpdateMeasuredData(0, 0, 0);
}

void menuConfigure(uint8_t brightness, uint8_t contrast) {
    lcdSetDisplayBrightness(brightness);
    lcdSetDisplayContrast(contrast);
}

void menuUpdateMeasuredData(uint16_t msrVoltage, uint16_t msrCurrent, uint16_t msrTemperature) {
    if (currentMenu != MEASURE_VALUES) {
        lcdCursorUnderlineOn(false);
        lcdTurnOnBlinkingCursor(false);
        writeString(0,0, "M|V[mV] I[mA] T ");
        writeString(1,0, " |xx,xx xx,xx xx");
        currentMenu = MEASURE_VALUES;
    }
    
    //drawValue(1, 2, msrVoltage);
    //drawValue(1, 8, msrCurrent);
    //drawValue(1, 14, msrVoltage);
}

void menuSelectVoltage(uint16_t selVoltage) {
    if (currentMenu != SELECT_VOLTAGE) {
        lcdCursorUnderlineOn(true);
        lcdTurnOnBlinkingCursor(false);
        lcdSetCursorPosition(0,15); // Underline 'V'
        writeString(0,0, "S|Set voltage  V");
        writeString(1,0, " |              ");
        currentMenu = SELECT_VOLTAGE;
    }
    drawValue(1, 2, selVoltage);
}

void menuSelectCurrent(uint16_t selCurrent) {
    if (currentMenu != SELECT_CURRENT) {
        lcdCursorUnderlineOn(true);
        lcdTurnOnBlinkingCursor(false);
        lcdSetCursorPosition(1,15); // Underline 'I'
        writeString(0,0, "S|Set current  I");
        writeString(1,0, " |              ");
        currentMenu = SELECT_CURRENT;
    }
    drawValue(1, 2, selCurrent);
}
    
void menuChangeVoltage(uint16_t selVoltage) {
    if (currentMenu != CHANGE_VOLTAGE) {
        lcdCursorUnderlineOn(true);
        lcdTurnOnBlinkingCursor(true);
        lcdSetCursorPosition(1,7);
        writeString(0,0, "C|Set voltage  V");
        writeString(1,0, " | xx,xx       V");
        currentMenu = CHANGE_VOLTAGE;
    }
    drawValue(1, 2, selVoltage);
}
    
void menuChangeCurrent(uint16_t selCurrent) {
    if (currentMenu != CHANGE_CURRENT) {
        lcdCursorUnderlineOn(true);
        lcdTurnOnBlinkingCursor(true);
        lcdSetCursorPosition(1,7);
        writeString(0,0, "C|Set current  I");
        writeString(1,0, " | xx,xx       A");
        currentMenu = CHANGE_CURRENT;
    }
    drawValue(1, 2, selCurrent);
}