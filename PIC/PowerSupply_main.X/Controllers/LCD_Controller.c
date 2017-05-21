#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>

#include "../Settings.h"
#include "../Headers/Menu.h"
#include "../../Common/MENU_Settings.h"

#include "COM_Driver.h"
#include "LCD_Controller.h"
/*******************************************************************************
 *          DEFINES
 ******************************************************************************/


/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static menu_t currentMenu;
static next_t *cursor;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/

void C_LCD_Init() {
    // Arrows
    varMnAr.id = ID_ARROW_SM_R;
    varMnAr.selected = NO;
    varMnAr.next.nextType = NOPE;

    sm1Ar.id = ID_ARROW_SM_R;
    sm1Ar.selected = NO;
    sm1Ar.next.nextType = SUBMENU;
    sm1Ar.next.next.nextSubMenu = &varSm2;

    sm2Ar.id = ID_ARROW_SM_L;
    sm2Ar.selected = NO;
    sm2Ar.next.nextType = SUBMENU;
    sm2Ar.next.next.nextSubMenu = &varSm1;

    // Fields
    voltageFld.id = ID_M0_SM0_F0;
    voltageFld.selected = CURSOR;
    voltageFld.next.nextType = FIELD;
    voltageFld.next.next.nextField = &currentFld;

    currentFld.id = ID_M0_SM0_F1;
    currentFld.selected = NO;
    currentFld.next.nextType = ARROW;
    currentFld.next.next.nextArrow = &sm1Ar;

    loadCurrentFld.id = ID_M0_SM1_F0;
    loadCurrentFld.selected = NO;
    loadCurrentFld.next.nextType = NOPE;

    temperatureFld.id = ID_M0_SM1_F1;
    temperatureFld.selected = NO;
    temperatureFld.next.nextType = NOPE;

    // Sub menus
    varSm1.id = ID_M0_SM0;
    varSm1.field = &voltageFld;

    varSm1.id = ID_M0_SM1;
    varSm1.field = &temperatureFld;

    // Menu
    varMn.id = ID_M0;
    varMn.subMenu = &varSm1;

    currentMenu = varMn;
    
    next_t nt;
    nt.nextType = FIELD;
    nt.next.nextField = &voltageFld;
    
    cursor = &nt;
}

void C_LCD_Enable(bool enable) {

}

void C_LCD_Next() {
    switch (cursor->nextType) {
        case NOPE:
            break;
        case MENU:
            break;
        case SUBMENU:
            break;
        case FIELD:
            cursor->nextType = cursor->next.nextField->next.nextType;
            switch (cursor->nextType) {
                case NOPE:
                    break;
                case MENU:
                    break;
                case SUBMENU:
                    break;
                case FIELD:
                    break;
                case ARROW:
                    cursor->next.nextArrow = cursor->next.nextField->next.next.nextArrow;
                    break;
            }
            break;
        case ARROW:
            break;
    }
}


/*******************************************************************************
 *          INTERRUPTS
 ******************************************************************************/



