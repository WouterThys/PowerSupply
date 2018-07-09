#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "MENU_Controller.h"
#include "SUPPLIES_Controller.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/

#define M1_LINE1 "V:     V |     V"
#define M1_LINE2 "I:     mA|     m"
#define M1_varVPOS {0,0}
#define M1_varIPOS {1,0}
#define M1_msrVPOS {0,10}
#define M1_msrIPOS {1,10}

#define MENU_MAX_ON 200

/** NEW NEW NEW */
typedef enum {
    MEASURE_VALUES,
    SELECT_VOLTAGE,
    SELECT_CURRENT,
    CHANGE_VOLTAGE,
    CHANGE_CURRENT
} Menu_e;
/** NEW NEW NEW */

typedef enum {
    OFF,
    SELECTING,
    CHANGING        
} CursorMode_t;

typedef struct {
    uint16_t line;
    uint16_t pos;
} Position_t;

typedef struct {
    uint16_t value;
    Position_t position;
} Value_t;

typedef struct CursorPos {
    Position_t pos;
    struct CursorPos * next;
    struct CursorPos * prev;
    Value_t * value;
} CursorPosition_t;

typedef struct {
    CursorPosition_t * position;
    CursorMode_t mode; 
} Cursor_t;

typedef struct {
    const char * line1;
    const char * line2;
    Value_t variable1;
    Value_t variable2;
    Value_t measure1;
    Value_t measure2;
} Menu_t;

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
static void drawMenu(Menu_t menu);
static void drawCursor(Cursor_t cursor);
static void drawPrettyValue(Value_t value);
static void drawRealValue(Value_t value);
static void drawValue(uint8_t line, uint8_t startPos, uint16_t value);

static void menuTurn(int16_t turns);
static void menuClicked();

static void timerEnable(bool enable);
static void doEncLogic();

/* Event function pointers */
static bool (*putCommand)(Command_t data);

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
// MENU
static Value_t varVoltage = {0,    {0,2}};
static Value_t varCurrent = {1000, {1,2}};
static Value_t msrVoltage = {0,    {0,10}};
static Value_t msrCurrent = {0,    {1,10}};
static CursorPosition_t vcp = {M1_varVPOS};
static CursorPosition_t icp = {M1_varIPOS};

static Menu_t varMenu;
static Cursor_t cursor;
static uint16_t menuOnCount = 0;

static bool updateMenu = false;

/** NEW NEW NEW */
static Menu_e currentMenu;
/** NEW NEW NEW */

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/

void __attribute__ ( (interrupt, no_auto_psv) ) _T2Interrupt(void) {
    if (_T2IF) {
        doEncLogic();
        _T2IF = 0; // Clear interrupt
    }
}

void doEncLogic() {
    encDriverService(); // Update data
    int16_t turns = encDriverGetValue();
    Button_e buttonState = encDriverGetButton();
    
    uint16_t vVar = varVoltage.value;
    uint16_t iVar = varCurrent.value;
    // 5V, 3.3V ...
    
    // Knob turned
    while (turns != 0) {
        if (turns > 0) {
            menuTurn(-1);
            turns--;
        } else {
            menuTurn(1);
            turns++;
        }
        updateMenu = true;
    }
    
    // Knob pressed
    switch(buttonState) {
        default:
        case Open: break;
        case Closed: break;
        case Pressed: break;
        case Held: break;
        case Released: break;
        case Clicked: 
            menuClicked();
            updateMenu = true;
            break;
        case DoubleClicked:
            updateMenu = true;
            break;
    }
    
    // Send commands to main
    if (updateMenu) {
         Command_t command;
         
         if (vVar != varVoltage.value) {
             command.command = C_SET_VOLTAGE;
             command.data = varVoltage.value;
             (*putCommand)(command);
         }
         
         if (iVar != varCurrent.value) {
             command.command = C_SET_CURRENT;
             command.data = varCurrent.value;
             (*putCommand)(command);
         }
    }
}

void drawMenu(Menu_t menu) {
    writeString(0,0, menu.line1);
    writeString(1,0, menu.line2);
    
    drawPrettyValue(menu.variable1);
    drawRealValue(menu.variable2);
    drawPrettyValue(menu.measure1);
    drawRealValue(menu.measure2);
}

void drawCursor(Cursor_t cursor) {
    Position_t p = cursor.position->pos;
    lcdSetCursorPosition(p.line, p.pos);
    
    switch(cursor.mode) {
        default:
        case OFF: 
            lcdCursorUnderlineOn(false);
            lcdTurnOnBlinkingCursor(false);
            break;
        case SELECTING: 
            lcdCursorUnderlineOn(true);
            lcdTurnOnBlinkingCursor(false); 
            break;
        case CHANGING: 
            lcdCursorUnderlineOn(true);
            lcdTurnOnBlinkingCursor(true); 
            break;
    }
}

// d0 d1 , d2 d3
void drawPrettyValue(Value_t value) {
    uint16_t p = value.position.pos;
    uint16_t l = value.position.line;
    uint16_t v = value.value / 10;
    
    uint16_t d3 = v % 10;
    v /= 10;
    uint16_t d2 = v % 10;
    v /= 10;
    uint16_t d1 = v % 10;
    v /= 10;
    uint16_t d0 = v % 10;
    
    if (d0 > 0) {
        writeDigit(l, p, d0);
    } else {
        writeChar(l, p, ' ');
    }
    p++; writeDigit(l, p, d1);
    p++;  writeChar(l, p, ',');
    p++; writeDigit(l, p, d2);
    p++; writeDigit(l, p, d3);
    
    Position_t cp = cursor.position->pos;
    lcdSetCursorPosition(cp.line, cp.pos);
}

void drawRealValue(Value_t value) {
    uint16_t p = value.position.pos;
    uint16_t l = value.position.line;
    uint16_t v = value.value;
    
    uint16_t d4 = v % 10;
    v /= 10;
    uint16_t d3 = v % 10;
    v /= 10;
    uint16_t d2 = v % 10;
    v /= 10;
    uint16_t d1 = v % 10;
    v /= 10;
    uint16_t d0 = v % 10;
    
    if (d0 > 0) { writeDigit(l, p, d0); }
    else { writeChar(l, p, ' '); }
    
    p++; 
    if (d0 > 0 || d1 > 0) { writeDigit(l, p, d1); }
    else { writeChar(l, p, ' '); }
    
    p++; 
    if (d0 > 0 || d1 > 0 || d2 > 0) { writeDigit(l, p, d2); }
    else { writeChar(l, p, ' '); }
    
    p++; 
    if (d0 > 0 || d1 > 0 || d2 > 0 || d3 > 0) { writeDigit(l, p, d3); }
    else { writeChar(l, p, ' '); }
    
    p++; 
    writeDigit(l, p, d4);
    
    Position_t cp = cursor.position->pos;
    lcdSetCursorPosition(cp.line, cp.pos);
}

void menuTurn(int16_t turns) {
    if ((turns != 0) && (cursor.mode != OFF)) {
        if (cursor.mode == SELECTING) {
            if (turns > 0) {
                cursor.position = cursor.position->prev;
            } else {
                cursor.position = cursor.position->next;
            }
        } else {
            uint16_t * val = &cursor.position->value->value;
            if (turns > 0) {
                if (*val > 100) *val -= 100;
                else if (*val > 10) *val -= 10;
            } else {
                if (*val > 100) *val += 100;
                else *val += 10;
            }
        }
    }
}

void menuClicked() {
    switch(cursor.mode) {
        default:
        case OFF:
            break;
        case SELECTING:
            cursor.mode = CHANGING;
            //drawCursor(cursor);
            break;
        case CHANGING:
            cursor.mode = SELECTING;
            //drawCursor(cursor);
            break;
    }
}

void menuTurnOnCursor(bool on) {
    switch(cursor.mode) {
        default:
        case OFF:
            if (on) { 
                cursor.mode = SELECTING;
                drawCursor(cursor);
            }
            break;
        case CHANGING:    
        case SELECTING:
            if (!on) {
                cursor.mode = OFF;
                drawCursor(cursor);   
            }
            break;
    }
}

void timerEnable(bool enable) {
    T2CONbits.TON = 0; // Disable
    T2CONbits.TCS = 0; // Internal clock (Fp)
    T2CONbits.T32 = 0; // 16-bit timer
    T2CONbits.TCKPS = 0b11; // 1:256
    
    // Registers
    TMR2 = 0x0000;
    PR2 = 70;
    
    // Interrupts
    _T2IP = IP_MAIN_TMR; 
    _T2IF = 0; // Clear
    _T2IE = 1; // Enable
    
    if (enable) {
        T2CONbits.TON = 1; // Enable
    } 
}

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/

void menuInit(bool (*onPutCommand)(Command_t data)) {
    lcdInit();
    
    // Event
    putCommand = onPutCommand;
    
    // Menus
    varMenu.line1 = M1_LINE1;
    varMenu.line2 = M1_LINE2;
    varMenu.variable1 = varVoltage;
    varMenu.variable2 = varCurrent;
    varMenu.measure1 = msrVoltage;
    varMenu.measure2 = msrCurrent;
    
    drawMenu(varMenu);
    
    // Cursor
    vcp.next = &icp;
    vcp.prev = &icp;
    vcp.value = &varVoltage;
    
    icp.next = &vcp;
    icp.prev = &vcp;
    icp.value = &varCurrent;
    
    cursor.mode = OFF;
    cursor.position = &vcp;
    drawCursor(cursor);
    
    encDriverInit();
    timerEnable(true);
}

void menuUpdate(
        LCD_Settings_t settings, 
        SupplyData_t dataVar) 
{
    
//    if (settings.changed) {
//        lcdSetDisplayBrightness(settings.brightness);
//        lcdSetDisplayContrast(settings.contrast);
//        settings.changed = 0;
//    }
    
    if (dataVar.msrVoltage.changed) {
        msrVoltage.value = dataVar.msrVoltage.value;
        drawPrettyValue(msrVoltage);
    }
    if (dataVar.msrCurrent.changed) {
        msrCurrent.value = dataVar.msrCurrent.value;
        drawRealValue(msrCurrent);
    }
    if (dataVar.setVoltage.changed /* && current menu has variable*/) {
        varVoltage.value = dataVar.setVoltage.value;
        drawPrettyValue(varVoltage);
    }
    if (dataVar.setCurrent.changed /* && current menu has variable*/) {
        varCurrent.value = dataVar.setCurrent.value;
        drawRealValue(varCurrent);
    }
    
    
    // if (data5V0 ...)
    // if (data3V3 ...)
    
    if (updateMenu) {
        menuTurnOnCursor(true);
        
        drawCursor(cursor);
        
        menuOnCount = 0;
        updateMenu = false;
        
    } else {
        
        if (menuOnCount < MENU_MAX_ON) {
            menuOnCount++;
        }
    
    }
    
    // Menu on count
    if (menuOnCount >= MENU_MAX_ON) {
        menuTurnOnCursor(false);
    }
}

void menuSetVariableVoltage(uint16_t voltage) {
    if (varVoltage.value != voltage) {
        Position_t p = cursor.position->pos;

        varVoltage.value = voltage;
        drawPrettyValue(varVoltage);

        lcdSetCursorPosition(p.line, p.pos);
    }
}
    
void menuSetVariableCurrent(uint16_t current) {
    if (varCurrent.value != current) {
        Position_t p = cursor.position->pos;

        varCurrent.value = current;
        drawPrettyValue(varCurrent); 

        lcdSetCursorPosition(p.line, p.pos);
    }
}

void menuSetMeasuredVoltage(uint16_t voltage) {
    if (msrVoltage.value != voltage) {
        Position_t p = cursor.position->pos;

        msrVoltage.value = voltage;
        drawPrettyValue(msrVoltage);

        lcdSetCursorPosition(p.line, p.pos);
    }
}

void menuSetMeasuredCurrent(uint16_t current) {
    if (msrCurrent.value != current) {
        Position_t p = cursor.position->pos;

        msrCurrent.value = current;
        drawPrettyValue(msrCurrent); 

        lcdSetCursorPosition(p.line, p.pos);
    }
}




void menuUpdateMeasuredData(uint16_t msrVoltage, uint16_t msrCurrent, uint16_t msrTemperature) {
    if (currentMenu != MEASURE_VALUES) {
        lcdCursorUnderlineOn(false);
        lcdTurnOnBlinkingCursor(false);
        writeString(0,0, "M|V[mV] I[mA] T ");
        writeString(1,0, " |xx,xx xx,xx xx");
        currentMenu = MEASURE_VALUES;
    }
    
    drawValue(1, 2, msrVoltage);
    drawValue(1, 8, msrCurrent);
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