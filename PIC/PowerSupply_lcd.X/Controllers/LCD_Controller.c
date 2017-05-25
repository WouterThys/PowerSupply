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
/**
 * Initial values
 */  

// Units
#define VOLTAGE     {ID_VOLTAGE,     "V", "mV"}
#define CURRENT     {ID_CURRENT,     "I", "mA"}
#define TEMPERATURE {ID_TEMPERATURE, "T", "°C"}

// Arrows 
#define M0_SM0_AR   {ID_M0_SM0_AR,  126, 0, 15}  /* 127 is right arrow  */     
#define M0_SM1_AR   {ID_M0_SM1_AR,  127, 0, 15}  /* 126 is left arrow   */     
#define M0_AR       {ID_M0_AR,      126, 1, 1}  

// Fields:           id,            units,       value, line, position
#define M0_SM0_F0   {ID_M0_SM0_F0,  VOLTAGE,     0.0,   0,    11} 
#define M0_SM0_F1   {ID_M0_SM0_F1,  CURRENT,     0.0,   1,    11}
#define M0_SM1_F0   {ID_M0_SM1_F0,  TEMPERATURE, 0.0,   0,    11} 
#define M0_SM1_F1   {ID_M0_SM1_F1,  CURRENT,     0.0,   1,    11} 

// Sub menus:    id,         name, field1,           field2,          arrow      
#define M0_SM0  {ID_M0_SM0,  "",   &voltageFld,      &currentFld,     &sm0Arw}
#define M0_SM1  {ID_M0_SM1,  "",   &loadCurrentFld,  &temperatureFld, &sm1Arw}

// Menus:    id,     name,  arrow
#define M0  {ID_M0,  "VAR", &m0Arw}

/*******************************************************************************
 *          LOCAL FUNCTION DEFINES
 ******************************************************************************/
static void lcdDrawField(field_t *field);
static void lcdDrawSubMenu(subMenu_t *subMenu);
static void lcdDrawMenu(menu_t *menu);

static uint16_t countDigits(int16_t value);

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/
#define clearScreen() D_LCD_ClearSreen()
#define writeString(x,y,str) D_LCD_Goto(x,y); D_LCD_WriteString(str) 
#define writeChar(x,y,c) D_LCD_Goto(x,y); D_LCD_WriteChar(c)
#define writeDouble(x,y,d,p) D_LCD_Goto(x,y); D_LCD_WriteDouble(d, p)

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/

/**
 * Assign
 */
static arrow_t sm0Arw = M0_SM0_AR;
static arrow_t sm1Arw = M0_SM1_AR;
static arrow_t m0Arw = M0_AR;
    
static field_t voltageFld = M0_SM0_F0;
static field_t currentFld = M0_SM0_F1;
static field_t loadCurrentFld = M0_SM1_F0;
static field_t temperatureFld = M0_SM1_F1;
    
static subMenu_t varSm1 = M0_SM0;
static subMenu_t varSm2 = M0_SM1;
    
static menu_t varMn = M0;

static bool drawing;

const static uint16_t menuCnt = 1;
const static uint16_t subMenuCnt = 2;
const static uint16_t fieldCnt = 4;

static menu_t    *menus[1];
static subMenu_t *subMenus[2];
static field_t   *fields[4];
static arrow_t   *arrows[3];

static int8_t selectedMenuId;
static int8_t selectedSubMenuId;
static int8_t selectedFieldId;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
void lcdDrawField(field_t *field) {
    bool clearDrawing = false;
    if (!drawing) {
        drawing = true;
        clearDrawing = true;
    }
    // Name
    writeString(field->line, 5, field->unit.symbol);
    writeChar(field->line, 6, ':');
    
    // Value and units
    double val = (double)field->value;
    uint16_t precision = 0;
    if (val < 1000) {
        switch (field->unit.id) {
            case ID_VOLTAGE:
                field->unit.units = "mV";
                break;
            case ID_CURRENT:
                field->unit.units = "mA";
                break;
            default:
                break;
        }
    } else {
        switch (field->unit.id) {
            case ID_VOLTAGE:
                field->unit.units = "V ";
                val /= 1000;
                if (val > 99) {
                    precision = 0;
                } else {
                    precision = 2;
                }
                break;
            case ID_CURRENT:
                field->unit.units = "A ";
                val /= 1000;
                if (val > 99) {
                    precision = 0;
                } else {
                    precision = 2;
                }
                break;
            default:
                break;
        }
    }
    
    // Calculate position on screen
    uint16_t digitsToDisplay = countDigits(val);
    uint16_t pos = 0;
    if (precision > 0) {
        digitsToDisplay += precision + 1; // Digits and ","
    }
    pos = (field->pos) - digitsToDisplay + 1;
    
    // First clear field
    writeString(field->line, 7, "     ");
    writeDouble(field->line, pos, val, precision);
    writeString(field->line, 12, field->unit.units);

    if (clearDrawing) {
        drawing = false;
    }
}

void lcdDrawSubMenu(subMenu_t *subMenu) {
    bool clearDrawing = false;
    if (!drawing) {
        drawing = true;
        clearDrawing = true;
    }
    // Name and arrow
    writeString(1, 13, subMenu->name);
    writeChar(subMenu->arrow->line, subMenu->arrow->pos, subMenu->arrow->arrow);

    // Draw fields
    lcdDrawField(subMenu->field1);
    lcdDrawField(subMenu->field2);

    // Go to field1 as default 
    D_LCD_Goto(subMenu->field1->line, subMenu->field1->pos);
    D_LCD_CursorStyle(true, false);

    if (clearDrawing) {
        drawing = false;
    }
}

void lcdDrawMenu(menu_t *menu) {
    bool clearDrawing = false;
    if (!drawing) {
        drawing = true;
        clearDrawing = true;
    }

    // Menu name and arrow
    writeString(0, 0, menu->name);
    writeChar(menu->arrow->line, menu->arrow->pos, menu->arrow->arrow);
    writeChar(0, 3, '|');
    writeChar(1, 3, '|');

    if (clearDrawing) {
        drawing = false;
    }
}

uint16_t countDigits(int16_t value) {
    uint16_t count = 1;
    if (value < 0) {
        value *= -1;
        count++; // Count minus sign as a digit
    }
    while (value >= 10) {
        count++;
        value /= 10;
    }
    return count;
}


/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/

void C_LCD_Init() {
    D_LCD_Init();
    D_LCD_Enable(true);
    D_LCD_CursorStyle(false, false);

    drawing = false;

    menus[varMn.id] = &varMn;

    subMenus[varSm1.id] = &varSm1;
    subMenus[varSm2.id] = &varSm2;

    fields[voltageFld.id]     = &voltageFld;
    fields[currentFld.id]     = &currentFld;
    fields[temperatureFld.id] = &temperatureFld;
    fields[loadCurrentFld.id] = &loadCurrentFld;
    
    arrows[sm0Arw.id] = &sm0Arw;
    arrows[sm1Arw.id] = &sm1Arw;
    arrows[m0Arw.id]  = &m0Arw;

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

    menu_t m = *menus[selectedMenuId];
    subMenu_t sm = *subMenus[selectedSubMenuId];
    
    switch (what) {
        case TYPE_ARROW: {
            // If arrow is visible
            if (sm.arrow->id == id || m.arrow->id == id) {
                D_LCD_Goto(arrows[id]->line, arrows[id]->pos);
                D_LCD_CursorStyle(true, select);
            }
        } break;
        case TYPE_FIELD: {
            // If field is visible
            if (sm.field1->id == id || sm.field2->id == id) {
                D_LCD_CursorStyle(true, select);
                D_LCD_Goto(fields[id]->line, fields[id]->pos);
            }
        } break;
        default:
            break;
    }

}

void C_LCD_SetFieldValue(uint8_t id, int16_t value) {
    if (id < fieldCnt) {
        switch (fields[id]->unit.id) {
            case ID_VOLTAGE:
                // Convert to voltage
                // ...
                fields[id]->value = value;
                break;
            case ID_CURRENT:
                // Convert to current
                // ...
                fields[id]->value = value;
                break;
            case ID_TEMPERATURE:
                // Convert to temperature
                // ...
                fields[id]->value = value;
                break;
            default:
                break;
        }

        subMenu_t sm = *subMenus[selectedSubMenuId];
        if (sm.field1->id == id || sm.field2->id == id) {
            lcdDrawField(fields[id]);
            D_LCD_Goto(fields[id]->line, fields[id]->pos);
        }

    }
}