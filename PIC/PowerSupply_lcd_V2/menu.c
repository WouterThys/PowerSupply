#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "Settings.h"
#include "Drivers/SYSTEM_Driver.h"
#include "Drivers/LCD_Driver.h"
#include "menu.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/

#define M1_LINE1 "V:     V |     V"
#define M1_LINE2 "I:     A |     A"
#define M1_varVPOS {0,0}
#define M1_varIPOS {1,0}
#define M1_msrVPOS {0,10}
#define M1_msrIPOS {1,10}

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
 *          LOCAL FUNCTION DEFINES
 ******************************************************************************/


/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/
#define clearScreen() lcdClearScreen()
#define writeString(x,y,str) lcdSetCursorPosition(x,y); lcdWriteString(str) 
#define writeChar(x,y,c) lcdSetCursorPosition(x,y); lcdWriteChar(c)
#define writeDigit(x,y,d) lcdSetCursorPosition(x,y); lcdWriteDigit(d)

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static Value_t varVoltage = {0,    {0,2}};
static Value_t varCurrent = {1000, {1,2}};
static Value_t msrVoltage = {0, {0,10}};
static Value_t msrCurrent = {0, {1,10}};
static CursorPosition_t vcp = {M1_varVPOS};
static CursorPosition_t icp = {M1_varIPOS};


static Menu_t varMenu;
static Cursor_t cursor;


/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void drawMenu(Menu_t menu);
static void drawCursor(Cursor_t cursor);
static void drawValue(Value_t value);



void drawMenu(Menu_t menu) {
    writeString(0,0, menu.line1);
    writeString(1,0, menu.line2);
    
    drawValue(menu.variable1);
    drawValue(menu.variable2);
    drawValue(menu.measure1);
    drawValue(menu.measure2);
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
void drawValue(Value_t value) {
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
}


/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/

void menuInit() {
    lcdInit();
    
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
    vcp.prev = &vcp;
    vcp.value = &varVoltage;
    
    icp.next = &vcp;
    icp.prev = &icp;
    icp.value = &varCurrent;
    
    cursor.mode = OFF;
    cursor.position = &vcp;
    drawCursor(cursor);
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
            } else {
                *val += 100;
            }
            drawValue(*cursor.position->value);
        }
        drawCursor(cursor);
    }
}

void menuClicked() {
    switch(cursor.mode) {
        default:
        case OFF:
            break;
        case SELECTING:
            cursor.mode = CHANGING;
            drawCursor(cursor);
            break;
        case CHANGING:
            cursor.mode = SELECTING;
            drawCursor(cursor);
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

void menuGetVoltage(uint16_t * v) {
    *v = varVoltage.value;
}

void menuGetCurrent(uint16_t * c) {
    *c = varCurrent.value;
}

void menuSetVariableVoltage(uint16_t voltage) {
    Position_t p = cursor.position->pos;
    
    varVoltage.value = voltage;
    drawValue(varVoltage);
    
    lcdSetCursorPosition(p.line, p.pos);
}
    
void menuSetVariableCurrent(uint16_t current) {
    Position_t p = cursor.position->pos;
    
    varCurrent.value = current;
    drawValue(varCurrent); 
    
    lcdSetCursorPosition(p.line, p.pos);
}

void menuSetMeasuredVoltage(uint16_t voltage) {
    Position_t p = cursor.position->pos;
    
    msrVoltage.value = voltage;
    drawValue(msrVoltage);
    
    lcdSetCursorPosition(p.line, p.pos);
}

void menuSetMeasuredCurrent(uint16_t current) {
    Position_t p = cursor.position->pos;
    
    msrCurrent.value = current;
    drawValue(msrCurrent); 
    
    lcdSetCursorPosition(p.line, p.pos);
}

