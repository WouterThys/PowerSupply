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

#define M1_LINE1 " |     V       V"
#define M1_LINE2 " |     I C     A"
#define M1_varVPOS {0,7}
#define M1_varIPOS {1,7}
#define M1_msrVPOS {0,10}
#define M1_msrIPOS {1,10}
#define M1_NPOS {1,0}

typedef enum {
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
Value_t voltage = {5000, {0,2}};
Value_t current = {1000, {1,2}};
Value_t msrVoltage = {0, {0,10}};
Value_t msrCurrent = {0, {1,10}};
CursorPosition_t vcp = {M1_varVPOS};
CursorPosition_t icp = {M1_varIPOS};
CursorPosition_t ncp = {M1_NPOS};


Menu_t varMenu;
Cursor_t cursor;


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
        case SELECTING: lcdTurnOnBlinkingCursor(false); break;
        case CHANGING: lcdTurnOnBlinkingCursor(true); break;
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
    lcdCursorUnderlineOn(true);
    
    // Menus
    varMenu.line1 = M1_LINE1;
    varMenu.line2 = M1_LINE2;
    varMenu.variable1 = voltage;
    varMenu.variable2 = current;
    varMenu.measure1 = msrVoltage;
    varMenu.measure2 = msrCurrent;
    
    drawMenu(varMenu);
    
    // Cursor
    vcp.next = &icp;
    vcp.prev = &ncp;
    vcp.value = &voltage;
    
    icp.next = &ncp;
    icp.prev = &vcp;
    icp.value = &current;
    
    ncp.next = &vcp;
    ncp.prev = &icp;
    
    cursor.mode = SELECTING;
    cursor.position = &vcp;
    drawCursor(cursor);
}

void menuTurn(int16_t turns) {
    if (turns != 0) {
        if (cursor.mode == SELECTING) {
            if (turns > 0) {
                cursor.position = cursor.position->prev;
            } else {
                cursor.position = cursor.position->next;
            }
        } else {
            if (turns > 0) {
                cursor.position->value->value -= 100;
            } else {
                cursor.position->value->value += 100;
            }
            drawValue(*cursor.position->value);
        }
        drawCursor(cursor);
    }
}

void menuClicked() {
    if (cursor.mode == SELECTING) {
        cursor.mode = CHANGING;
    } else {
        cursor.mode = SELECTING;
    }
    
    drawCursor(cursor);
}

