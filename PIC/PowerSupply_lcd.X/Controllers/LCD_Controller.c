#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "../Settings.h"
#include "../Drivers/SYSTEM_Driver.h"
#include "../Drivers/LCD_Driver.h"
#include "../Drivers/Menu.h"
#include "LCD_Controller.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/


/*******************************************************************************
 *          LOCAL FUNCTION DEFINES
 ******************************************************************************/
static void lcdDrawField(field_t field);
static void lcdDrawSubMenu(subMenu_t subMenu);
static void lcdDrawMenu(menu_t menu);

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/
#define clearScreen() D_LCD_ClearSreen()
#define writeString(x,y,str) D_LCD_Goto(x,y); D_LCD_WriteString(str) 
#define writeChar(x,y,c) D_LCD_Goto(x,y); D_LCD_WriteChar(c)
#define writeDouble(x,y,d) D_LCD_Goto(x,y); D_LCD_WriteDouble(d, 2)

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static bool drawing;

const static uint16_t menuCnt = 1;
const static uint16_t subMenuCnt = 2;
const static uint16_t fieldCnt = 4;

static menu_t menus[1];
static subMenu_t subMenus[2];
static field_t fields[4];

static int8_t selectedMenuId;
static int8_t selectedSubMenuId;
static int8_t selectedFieldId;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
void lcdDrawField(field_t field) {
    bool clearDrawing = false;
    if (!drawing) {
        drawing = true;
        clearDrawing = true;
    }
    // Name, value, units
    writeString(field.line, 5, field.unit.symbol);
    writeChar(field.line, 6, ':');
    uint8_t pos = 0;
    if (field.value >= 10) {
        pos = 7;
    } else {
        pos = 8;
    }
    writeDouble(field.line, pos, field.value);
    writeString(field.line, 12, field.unit.units);

    if (clearDrawing) {
        drawing = false;
    }
}

void lcdDrawSubMenu(subMenu_t subMenu) {
    bool clearDrawing = false;
    if (!drawing) {
        drawing = true;
        clearDrawing = true;
    }
    // Name and arrow
    writeString(1, 13, subMenu.name);
    writeChar(subMenu.arrow->line, subMenu.arrow->pos, subMenu.arrow->arrow);

    // Draw fields
    lcdDrawField(*subMenu.field1);
    lcdDrawField(*subMenu.field2);
    
    // Go to field1 as default 
    D_LCD_Goto(subMenu.field1->line, subMenu.field1->pos);
    D_LCD_CursorStyle(true, false);

    if (clearDrawing) {
        drawing = false;
    }
}

void lcdDrawMenu(menu_t menu) {
    bool clearDrawing = false;
    if (!drawing) {
        drawing = true;
        clearDrawing = true;
    }

    // Menu name and arrow
    writeString(0, 0, menu.name);
    writeChar(menu.arrow->line, menu.arrow->pos, menu.arrow->arrow);
    writeChar(0, 3, '|');
    writeChar(1, 3, '|');

    if (clearDrawing) {
        drawing = false;
    }
}

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/

void C_LCD_Init() {
    D_LCD_Init();
    D_LCD_Enable(true);
    D_LCD_CursorStyle(false, false);

    drawing = false;

    menus[0] = varMn;

    subMenus[0] = varSm1;
    subMenus[1] = varSm2;

    fields[0] = voltageFld;
    fields[1] = currentFld;
    fields[2] = temperatureFld;
    fields[3] = loadCurrentFld;

    selectedMenuId = -1;
    selectedSubMenuId = -1;
    selectedFieldId = -1;

    C_LCD_DrawMenu(varMn.id);
    C_LCD_DrawSubMenu(varSm1.id);
    C_LCD_SetSelected(1, voltageFld.id, false);
}

void C_LCD_DrawMenu(uint8_t id) {
    if (id < menuCnt) {
        selectedMenuId = id;
        lcdDrawMenu(menus[id]);
    }
}

void C_LCD_DrawSubMenu(uint8_t id) {
    if (id < subMenuCnt) {
        selectedSubMenuId = id;
        lcdDrawSubMenu(subMenus[id]);
    }
}

void C_LCD_SetSelected(uint8_t what, uint8_t id, bool select) {
    if (id < fieldCnt) {
        subMenu_t sm = subMenus[selectedSubMenuId];

        switch (what) {
            case TYPE_ARROW:
                D_LCD_Goto(sm.arrow->line, sm.arrow->pos);
                D_LCD_CursorStyle(true, select);
                break;
            case TYPE_FIELD:
                if (sm.field1->id == id || sm.field2->id == id) {
                    D_LCD_CursorStyle(true, select);
                    D_LCD_Goto(fields[id].line, fields[id].pos);
                }
                break;
            default:
                break;
        }
    }
}

void C_LCD_SetFieldValue(uint8_t id, int16_t value) {
    if (id < fieldCnt) {
        switch (fields[id].unit.id) {
            case ID_VOLTAGE:
                // Convert to voltage
                // ...
                fields[id].value = (double) value;
                break;
            case ID_CURRENT:
                // Convert to current
                // ...
                fields[id].value = (double) value;
                break;
            case ID_TEMPERATURE:
                // Convert to temperature
                // ...
                fields[id].value = (double) value;
                break;
            default:
                break;
        }

        subMenu_t sm = subMenus[selectedSubMenuId];
        if (sm.field1->id == id || sm.field2->id == id) {
            lcdDrawField(fields[id]);
            D_LCD_Goto(fields[id].line, fields[id].pos);
        }

    }
}