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
#define ARROW_L 127
#define ARROW_R 126
#define ARROW_U 0b00011110
#define ARROW_D 0b00011111

/*******************************************************************************
 *          LOCAL FUNCTION DEFINES
 ******************************************************************************/
static void lcdDrawField(menuField_t field);
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
static menu_t currentMenu;
static bool drawing;

static menuField_t voltageField, currentField, loadCurrentField, temperatureField;
static subMenu_t subMenu1, subMenu2;
static menu_t variableMenu;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
void lcdDrawField(menuField_t field) {
    bool clearDrawing = false;
    if (!drawing) {
        drawing = true;
        clearDrawing = true;
    }
    // Name, value, units
    writeString(field.line,5, field.name);
    writeChar(field.line,6, ':');
    writeDouble(field.line,7, field.value);
    D_LCD_WriteString(field.units);
    
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
    writeChar(0,15, ARROW_R);
    
    // Draw fields
    lcdDrawField(*subMenu.field1);
    lcdDrawField(*subMenu.field2);
    
    if (clearDrawing) {
        drawing = false;
    }
}

void lcdDrawMenu(menu_t menu) {
    if (!drawing) {
        drawing = true;
    }
    clearScreen(); // Clear and go to origin
    
    // Menu name and arrow
    writeString(0,0,menu.name);
    writeChar(1,1, ARROW_R);
    writeChar(0,3, '|');
    writeChar(1,3, '|');
    
    // Draw sub menus
    lcdDrawSubMenu(*menu.subMenu1);
    drawing = false;
}

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/

void C_LCD_Init() {
    D_LCD_Init();
    D_LCD_Enable(true);
    D_LCD_CursorStyle(false, false);
    
    drawing = false;
    
//    int16_t i = 0;
//    // TEST
//    while(1) {
//        D_LCD_Goto(0,0);
//        D_LCD_WriteInt(i);
//        D_LCD_Goto(1,0);
//        D_LCD_WriteChar(i);
//        i++;
//        DelayMs(200);
//    }
    
    // Fields
    voltageField.id = 0;
    voltageField.name = "V";
    voltageField.units = "mV";
    voltageField.line = 0;
    voltageField.value = 1.25;
    voltageField.selectable = true;
    voltageField.selected = true;
    
    currentField.id = 1;
    currentField.name = "I";
    currentField.units = "mA";
    currentField.line = 1;
    currentField.value = 0.12;
    currentField.selectable = true;
    currentField.selected = false;
    
    loadCurrentField.id = 2;
    loadCurrentField.name = "I";
    loadCurrentField.units = "mA";
    loadCurrentField.line = 0;
    loadCurrentField.value = 0.08;
    loadCurrentField.selectable = false;
    loadCurrentField.selected = false;
    
    temperatureField.id = 3;
    temperatureField.name = "T";
    temperatureField.units = "°C";
    temperatureField.line = 1;
    temperatureField.value = 85;
    temperatureField.selectable = false;
    temperatureField.selected = false;
    
    
    voltageField.nextField = &currentField;
    currentField.nextField = &voltageField;
    loadCurrentField.nextField = NULL;
    temperatureField.nextField = NULL;
    
    // Sub menus
    subMenu1.id = 0;
    subMenu1.name = "";
    subMenu1.selected = true;
    subMenu1.field1 = &voltageField;
    subMenu1.field2 = &currentField;
    
    subMenu2.id = 1;
    subMenu2.name = "";
    subMenu2.selected = false;
    subMenu2.field1 = &loadCurrentField;
    subMenu2.field2 = &temperatureField;
    
    subMenu1.nexSubMenu = &subMenu2;
    subMenu2.nexSubMenu = &subMenu1;
    
    // Menus
    variableMenu.id = 0;
    variableMenu.name = "VAR";
    variableMenu.selected = false;
    variableMenu.subMenu1 = &subMenu1;
    variableMenu.subMenu2 = &subMenu2;
    variableMenu.nextMenu = NULL;
    
    lcdDrawMenu(variableMenu);
    currentMenu = variableMenu;
    currentMenu.selectedSubMenu = &subMenu1;
}

void C_LCD_NextMenu() {
    
}

void C_LCD_NextSubMenu() {
    if (!drawing) {
        currentMenu.selectedSubMenu = currentMenu.selectedSubMenu->nexSubMenu;
        lcdDrawSubMenu(*currentMenu.selectedSubMenu);
    }
}