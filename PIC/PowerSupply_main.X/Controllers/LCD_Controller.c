#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>

#include "../Settings.h"
#include "../Headers/Menu.h"

#include "../Drivers/ENC_Driver.h"

#include "COM_Driver.h"
#include "LCD_Controller.h"
/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
/**
 * Initial values
 */
// Arrows
#define M0_SM0_AR   {ID_M0_SM0_AR, NS, {ARROW, ID_M0_AR},    {FIELD, ID_M0_SM0_F1}}
#define M0_SM1_AR   {ID_M0_SM1_AR, NS, {NOTHING, -1},        {NOTHING, -1}}
#define M0_AR       {ID_M0_AR   ,  NS, {FIELD, ID_M0_SM0_F0},{ARROW, ID_M0_SM0_AR}}

// Fields
#define M0_SM0_F0   {ID_M0_SM0_F0, 0, 0, 100,30000, OUTPUT_VOLTAGE, CURSOR,         {FIELD, ID_M0_SM0_F1},  {ARROW, ID_M0_AR}}  /* Voltage field */
#define M0_SM0_F1   {ID_M0_SM0_F1, 0, 50,50, 1500,  CURRENT_LIMIT,  NS,             {ARROW, ID_M0_SM0_AR},  {FIELD, ID_M0_SM0_F0}} /* Current field */
#define M0_SM1_F0   {ID_M0_SM1_F0, 0, 0, 0,  0,     TEMPERATURE,    NOT_SELECTABLE, {NOTHING, -1}, {NOTHING, -1}} /* Temperature field */
#define M0_SM1_F1   {ID_M0_SM1_F1, 0, 0, 0,  0,     CURRENT_LOAD,   NOT_SELECTABLE, {NOTHING, -1}, {NOTHING, -1}} /* Load current field */

// Sub menus
#define M0_SM0  {ID_M0_SM0, ID_M0_SM0_F0, ID_M0_SM0_F1, ID_M0_SM0_AR, ID_M0_SM1}
#define M0_SM1  {ID_M0_SM1, ID_M0_SM1_F0, ID_M0_SM1_F1, ID_M0_SM1_AR, ID_M0_SM0}

// Menus
#define M0      {ID_M0, ID_M0_SM0, ID_M0_SM1, ID_M0_AR, -1}

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

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
static field_t temperatureFld = M0_SM1_F0;
static field_t loadCurrentFld = M0_SM1_F1;

static subMenu_t varSm0 = M0_SM0;
static subMenu_t varSm1 = M0_SM1;

static menu_t varMn = M0;

static menu_t menus[1];
static subMenu_t subMenus[2];
static field_t fields[4];
static arrow_t arrows[4];


/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void setNext(cursor_t *cursor, enc_t encValue);
static void setSelected(cursor_t *cursor);
static void setValueFromTurn(cursor_t *cursor, enc_t encValue);
static void setNewSubMenu(cursor_t *cursor, int8_t newSubMenuId);

static void nextFromArrow(cursor_t *cursor, enc_t encValue);
static void nextFromField(cursor_t *cursor, enc_t encValue);
static void next(cursor_t *cursor);

void setNext(cursor_t *cursor, enc_t encValue) {
    switch (cursor->currentType) {
        case ARROW:
            nextFromArrow(cursor, encValue);
            break;
        case FIELD:
            nextFromField(cursor, encValue);
            break;
        default:
            break;
    }
}

void setSelected(cursor_t *cursor) {
    switch (cursor->currentType) {
        case ARROW:
            // Go to next menu or sub-menu
            if (cursor->current.arrow->selected == CURSOR) {
                cursor->current.arrow->selected = NS;

                // Is menu arrow
                if (cursor->currentMenu->arrowId == cursor->current.arrow->id) {

                }                    // Id Sub-Menu arrow
                else if (cursor->currentSubMenu->arrowId == cursor->current.arrow->id) {
                    int8_t id = cursor->currentSubMenu->nextSubMenuId;
                    if (id >= 0) {
                        setNewSubMenu(cursor, id);
                    }
                }
            }
            break;
        case FIELD:
            // Select or deselect the field
            if (cursor->current.field->selected == CURSOR) {
                cursor->current.field->selected = SELECTED;
            } else if (cursor->current.field->selected == SELECTED) {
                cursor->current.field->selected = CURSOR;
            }
            // Set actions (selected state will be read when sending to LCD)
            cursor->nextActions[cursor->nextActionCnt] = SELECT;
            cursor->nextActionCnt++;
            break;
        default:
            break;
    }
}

void setNewSubMenu(cursor_t *cursor, int8_t newSubMenuId) {
    subMenu_t *nextSubMenu = &subMenus[newSubMenuId];
    
    // Set new sub menu
    cursor->currentSubMenu = nextSubMenu;
    
    // Set first selectable item to current
    if (fields[nextSubMenu->field1Id].selected != NOT_SELECTABLE) {
        field_t *firstField = &fields[nextSubMenu->field1Id];
        cursor->currentId = firstField->id;
        cursor->currentType = FIELD;
        cursor->current.field = firstField;
        cursor->current.field->selected = CURSOR;
    } else if (fields[nextSubMenu->field2Id].selected != NOT_SELECTABLE) {
        field_t *firstField = &fields[nextSubMenu->field2Id];
        cursor->currentId = firstField->id;
        cursor->currentType = FIELD;
        cursor->current.field = firstField;
        cursor->current.field->selected = CURSOR;
    } else {
        arrow_t *firstArrow = &arrows[nextSubMenu->arrowId];
        cursor->currentId = firstArrow->id;
        cursor->currentType = ARROW;
        cursor->current.arrow = firstArrow;
        cursor->current.arrow->selected = CURSOR;
    }

    // Actions
    cursor->nextActions[cursor->nextActionCnt] = DRAW;
    cursor->nextActionCnt++;
    cursor->nextActions[cursor->nextActionCnt] = SELECT;
    cursor->nextActionCnt++;
}

void setValueFromTurn(cursor_t *cursor, enc_t encValue) {
    if (encValue.turn == RIGHT) {
        cursor->current.field->value += (encValue.turnCount * cursor->current.field->step);
        if (cursor->current.field->value > cursor->current.field->max) {
            cursor->current.field->value = cursor->current.field->max;
        }
    } else {
        cursor->current.field->value -= (encValue.turnCount * cursor->current.field->step);
        if (cursor->current.field->value < cursor->current.field->min) {
            cursor->current.field->value = cursor->current.field->min;
        }
    }
    
    // Actions
    cursor->nextActions[cursor->nextActionCnt] = SET;
    cursor->nextActionCnt++;
}

void nextFromArrow(cursor_t *cursor, enc_t encValue) {
    // Go to next
    if (encValue.turn == RIGHT && cursor->current.arrow->next.id >= 0) { // Go forward
        // Deselect
        cursor->current.arrow->selected = NS;
        // Arrow can go to other field, sub-menu or menu
        switch (cursor->current.arrow->next.type) {
            case ARROW ... FIELD:
                cursor->nextActions[cursor->nextActionCnt] = SELECT;
                cursor->nextActionCnt++;
                break;
            case SUBMENU ... MENU:
                cursor->nextActions[cursor->nextActionCnt] = DRAW;
                cursor->nextActionCnt++;
                break;
            default:
                break;
        }
        cursor->currentType = cursor->current.arrow->next.type;
        cursor->currentId = cursor->current.arrow->next.id;

    } else if (encValue.turn == LEFT && cursor->current.arrow->previous.id >= 0) { // Go backward
        // Deselect
        cursor->current.arrow->selected = NS;
        // Arrow can go to other field, sub-menu or menu
        switch (cursor->current.arrow->previous.type) {
            case ARROW ... FIELD:
                cursor->nextActions[cursor->nextActionCnt] = SELECT;
                cursor->nextActionCnt++;
                break;
            case SUBMENU ... MENU:
                cursor->nextActions[cursor->nextActionCnt] = DRAW;
                cursor->nextActionCnt++;
                break;
            default:
                break;
        }
        cursor->currentType = cursor->current.arrow->previous.type;
        cursor->currentId = cursor->current.arrow->previous.id;
    }
    next(cursor);
}

void nextFromField(cursor_t *cursor, enc_t encValue) {
    // Field always goes to other field or arrow, LCD should only select
    cursor->nextActions[cursor->nextActionCnt] = SELECT;
    cursor->nextActionCnt++;

    if (encValue.turn == RIGHT) { // Go forward
        // Update values
        cursor->currentType = cursor->current.field->next.type;
        cursor->currentId = cursor->current.field->next.id;

    } else {
        // Update values
        cursor->currentType = cursor->current.field->previous.type;
        cursor->currentId = cursor->current.field->previous.id;
    }
    next(cursor);
}

void next(cursor_t *cursor) {
    switch (cursor->currentType) {
        case ARROW:
            cursor->current.arrow = &arrows[cursor->currentId];
            cursor->current.arrow->selected = CURSOR;
            break;
        case FIELD:
            cursor->current.field = &fields[cursor->currentId];
            cursor->current.field->selected = CURSOR;
            break;
        default:
            break;
    }
}

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/

void C_LCD_Init(cursor_t *cursor) {
    // Arrays
    arrows[sm0Arw.id] = sm0Arw;
    arrows[sm1Arw.id] = sm1Arw;
    arrows[m0Arw.id] = m0Arw;

    fields[voltageFld.id] = voltageFld;
    fields[currentFld.id] = currentFld;
    fields[temperatureFld.id] = temperatureFld;
    fields[loadCurrentFld.id] = loadCurrentFld;

    subMenus[varSm0.id] = varSm0;
    subMenus[varSm1.id] = varSm1;

    menus[varMn.id] = varMn;

    // Cursor
    cursor->currentId = voltageFld.id;
    cursor->currentType = FIELD;
    cursor->current.field = &voltageFld;

    cursor->previousType = ARROW;
    cursor->previous.arrow = &m0Arw;

    cursor->currentMenu = &varMn;
    cursor->currentSubMenu = &varSm0;

    cursor->nextActionCnt = 0;
}

void C_LCD_Enable(bool enable) {

}

void C_LCD_Turn(cursor_t *cursor, enc_t encValues) {
    if (cursor->currentType == FIELD) {
        if (cursor->current.field->selected == SELECTED) {
            setValueFromTurn(cursor, encValues);
        } else {
            setNext(cursor, encValues);
        }
    } else {
        setNext(cursor, encValues);
    }
}

void C_LCD_Press(cursor_t *cursor) {
    setSelected(cursor);
}


/*******************************************************************************
 *          INTERRUPTS
 ******************************************************************************/



