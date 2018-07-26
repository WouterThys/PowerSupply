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
    CHANGE_CURRENT,
    SELECT_CALIBRATION,
    CHANGE_CALIBRATION,
    SELECT_SETTINGS,
    CHANGE_SETTINGS
} Menu_e;

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/
#define clearScreen() lcdClearScreen()
#define writeString(x,y,str) lcdSetCursorPosition(x,y); lcdWriteString(str) 
#define writeChar(x,y,c) lcdSetCursorPosition(x,y); lcdWriteChar(c)
#define writeDigit(x,y,d) lcdSetCursorPosition(x,y); lcdWriteDigit(d)
#define writeInt(x,y,d) lcdSetCursorPosition(x,y); lcdWriteInt(d)
#define writeFloat(x,y,f) lcdSetCursorPosition(x,y); lcdWriteDouble(f, 3)

/*******************************************************************************
 *          LOCAL FUNCTION DEFINES
 ******************************************************************************/
static void drawValue(uint8_t line, uint8_t startPos, float value);
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
void drawValue(uint8_t line, uint8_t startPos, float value) {
//    uint16_t v = value / 10;
//    uint8_t p = startPos;
//    
//    uint16_t d3 = v % 10;
//    v /= 10;
//    uint16_t d2 = v % 10;
//    v /= 10;
//    uint16_t d1 = v % 10;
//    v /= 10;
//    uint16_t d0 = v % 10;
//    
//    writeDigit(line, p++, d0);
//    writeDigit(line, p++, d1);
//    writeChar(line, p++, ',');
//    writeDigit(line, p++, d2);
//    writeDigit(line, p, d3);
    
    writeFloat(line, startPos, (double) value);
    
}

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/

void menuInit(bool (*onPutCommand)(Command_t data)) {
    lcdInit();
    
    // Event
    putCommand = onPutCommand;
    
    // Initial value
    menuUpdateMeasuredData(0, 0, 0);
    currentMenu = INITIAL;
}

void menuConfigure(uint8_t brightness, uint8_t contrast) {
    lcdSetDisplayBrightness(brightness);
    lcdSetDisplayContrast(contrast);
}

void menuUpdateMeasuredData(uint16_t msrVoltage, uint16_t msrCurrent, uint16_t msrTemperature) {
    
    if (currentMenu != MEASURE_VALUES) {
        lcdCursorUnderlineOn(false);
        lcdTurnOnBlinkingCursor(false);
        writeString(0,0, "M|V:          V ");
        writeString(1,0, " |I:          A ");
        currentMenu = MEASURE_VALUES;
    }
    
    drawValue(0, 4, digitalToVoltage(msrVoltage));
    drawValue(1, 4, digitalToCurrent(msrCurrent));
  
}

void menuSelectVoltage(uint16_t selVoltage) {
    if (currentMenu == SELECT_CURRENT) { // Only change cursor position
        drawValue(0, 13, digitalToVoltage(selVoltage));
        lcdSetCursorPosition(0,2); // Underline 'V'
    } else {
        // Draw menu
        writeString(0,0, "S|Set V:       V");
        writeString(1,0, " |Set I:       A");
        drawValue(0, 13, digitalToVoltage(selVoltage));
       
        lcdCursorUnderlineOn(true);
        lcdTurnOnBlinkingCursor(false);
        lcdSetCursorPosition(0,2); // Underline 'V'

    }
    currentMenu = SELECT_VOLTAGE;
}

void menuSelectCurrent(uint16_t selCurrent) {
    if (currentMenu == SELECT_VOLTAGE) { // Only change cursor position
        drawValue(1, 13, digitalToVoltage(selCurrent)); 
        lcdSetCursorPosition(1,2); // Underline 'I'
    } else {
        // Draw menu
        writeString(0,0, "S|Set V:       V");
        writeString(1,0, " |Set I:       A");
        drawValue(1, 13, digitalToVoltage(selCurrent));
       
        lcdCursorUnderlineOn(true);
        lcdTurnOnBlinkingCursor(false);
        lcdSetCursorPosition(1,2); // Underline 'I'

    }
    currentMenu = SELECT_CURRENT;
}

 void menuSelectCalibration() {
    if (currentMenu == SELECT_SETTINGS) { // Only change cursor position
        lcdSetCursorPosition(0,2); 
    } else {
        // Draw menu 
        writeString(0,0, "S|Calibration   ");
        writeString(1,0, " |Settings      ");
        
        lcdCursorUnderlineOn(true);
        lcdTurnOnBlinkingCursor(false);
        lcdSetCursorPosition(0,2); 
    }
    currentMenu = SELECT_CALIBRATION;
 }
    
 void menuSelectSettings() {
    if (currentMenu == SELECT_CALIBRATION) { // Only change cursor position
        lcdSetCursorPosition(1,2); 
    } else {
        // Draw menu 
        writeString(0,0, "S|Calibration   ");
        writeString(1,0, " |Settings      ");
        
        lcdCursorUnderlineOn(true);
        lcdTurnOnBlinkingCursor(false);
        lcdSetCursorPosition(1,2); 
    }
    currentMenu = SELECT_SETTINGS;
 }
    
    
void menuChangeVoltage(uint16_t selVoltage) {
    if (currentMenu != CHANGE_VOLTAGE) {
        lcdCursorUnderlineOn(true);
        lcdTurnOnBlinkingCursor(true);
        lcdSetCursorPosition(1,7);
        writeString(0,0, "C|Set voltage  V");
        writeString(1,0, " |             V");
        currentMenu = CHANGE_VOLTAGE;
    }
    drawValue(1, 2, digitalToVoltage(selVoltage));
}
    
void menuChangeCurrent(uint16_t selCurrent) {
    if (currentMenu != CHANGE_CURRENT) {
        lcdCursorUnderlineOn(true);
        lcdTurnOnBlinkingCursor(true);
        lcdSetCursorPosition(1,7);
        writeString(0,0, "C|Set current  I");
        writeString(1,0, " |             A");
        currentMenu = CHANGE_CURRENT;
    }
    drawValue(1, 2, digitalToCurrent(selCurrent));
}

void menuChangeCalibration(uint16_t targetVoltage, uint16_t calibratedVoltage) {
    if (currentMenu != CHANGE_CALIBRATION) {
        lcdCursorUnderlineOn(false);
        lcdTurnOnBlinkingCursor(false);
        writeString(0,0, "A|Set to       V");
        writeString(1,0, " |             V");
        currentMenu = CHANGE_CALIBRATION;
    }
    drawValue(0, 8, digitalToVoltage(targetVoltage));
    drawValue(1, 2, digitalToVoltage(calibratedVoltage));
}
    
void menuChangeSettings() {
    if (currentMenu != CHANGE_SETTINGS) {
        lcdCursorUnderlineOn(false);
        lcdTurnOnBlinkingCursor(false);
        writeString(0,0, "X|Settings     V");
        writeString(1,0, " |             V");
        currentMenu = CHANGE_SETTINGS;
    }
}