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
    MEASURE1_VALUES,
    MEASURE2_VALUES,
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
#define writeFloat(x,y,f,p) lcdSetCursorPosition(x,y); lcdWriteDouble(f, p)

/*******************************************************************************
 *          LOCAL FUNCTION DEFINES
 ******************************************************************************/

/* Event function pointers */
static bool (*putCommand)(Command_t data);

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static Menu_e currentMenu;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/


/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/

void menuInit(bool (*onPutCommand)(Command_t data)) {
    lcdInit();
    
    // Event
    putCommand = onPutCommand;
    
    // Initial value
    menuUpdateMeasured1Data(0, 0, 0);
    currentMenu = INITIAL;
}

void menuConfigure(uint8_t brightness, uint8_t contrast) {
    lcdSetDisplayBrightness(brightness);
    lcdSetDisplayContrast(contrast);
}

void menuUpdateMeasured1Data(uint16_t msrVoltage, uint16_t msrCurrent, bool clip) {
    if (currentMenu != MEASURE1_VALUES) {
        lcdCursorUnderlineOn(false);
        lcdTurnOnBlinkingCursor(false);
        writeString(0,0, "M|V:          V ");
        writeString(1,0, " |I:          mA");
        currentMenu = MEASURE1_VALUES;
        
        if (DEBUG_LCD) printf("D%d\n", currentMenu);
    }
    
    writeFloat(0, 5, digitalToVoltage(msrVoltage), 2);
    writeFloat(1, 5, digitalToCurrent(msrCurrent), 2);
    
    if (clip) {
        writeString(1, 11, "C!");
    } else {
        writeString(1, 11, "  ");
    }
}

void menuUpdateMeasured2Data(uint16_t msrTemp, uint16_t msrCurrent_) {
    if (currentMenu != MEASURE2_VALUES) {
        lcdCursorUnderlineOn(false);
        lcdTurnOnBlinkingCursor(false);
        writeString(0,0, "M|T:          °C");
        writeString(1,0, " |i:          mA");
        currentMenu = MEASURE2_VALUES;
        
        if (DEBUG_LCD) printf("D%d\n", currentMenu);
    }
    
    writeFloat(0, 5, digitalToTemp(msrTemp), 2);
    writeFloat(1, 5, digitalToCurrent(msrCurrent_), 2);
}

void menuSelectVoltage(uint16_t selVoltage) {
    if (currentMenu == SELECT_CURRENT) { // Only change cursor position
        writeFloat(0, 11, digitalToVoltage(selVoltage), 2);
        lcdSetCursorPosition(0,2); // Underline 'V'
    } else if (currentMenu != SELECT_VOLTAGE) {
        // Draw menu
        writeString(0,0, "S|Set V:       V");
        writeString(1,0, " |Set I:      mA");
        writeFloat(0, 11, digitalToVoltage(selVoltage), 2);
       
        lcdCursorUnderlineOn(true);
        lcdTurnOnBlinkingCursor(false);
        lcdSetCursorPosition(0,2); // Underline 'V'

    }
    currentMenu = SELECT_VOLTAGE;
}

void menuSelectCurrent(uint16_t selCurrent) {
    if (currentMenu == SELECT_VOLTAGE) { // Only change cursor position
        writeFloat(1, 11, digitalToCurrent(selCurrent), 2);
        lcdSetCursorPosition(1,2); // Underline 'I'
    } else if (currentMenu != SELECT_CURRENT) {
        // Draw menu
        writeString(0,0, "S|Set V:       V");
        writeString(1,0, " |Set I:      mA");
        writeFloat(1, 11, digitalToCurrent(selCurrent), 2);
       
        lcdCursorUnderlineOn(true);
        lcdTurnOnBlinkingCursor(false);
        lcdSetCursorPosition(1,2); // Underline 'I'

    }
    currentMenu = SELECT_CURRENT;
}

 void menuSelectCalibration() {
    if (currentMenu == SELECT_SETTINGS) { // Only change cursor position
        lcdSetCursorPosition(0,2); 
    } else if (currentMenu != SELECT_CALIBRATION) {
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
    } else if (currentMenu != SELECT_SETTINGS) {
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
    writeFloat(1, 2, digitalToVoltage(selVoltage), 2);
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
    writeFloat(1, 2, digitalToCurrent(selCurrent), 2);
}

void menuChangeCalibration(uint16_t targetVoltage, uint16_t calibratedVoltage) {
    if (currentMenu != CHANGE_CALIBRATION) {
        lcdCursorUnderlineOn(false);
        lcdTurnOnBlinkingCursor(false);
        writeString(0,0, "A|Set to       V");
        writeString(1,0, " |             V");
        currentMenu = CHANGE_CALIBRATION;
    }
    writeFloat(0, 9, digitalToVoltage(targetVoltage), 2);
    writeFloat(1, 3, digitalToVoltage(calibratedVoltage), 2);
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