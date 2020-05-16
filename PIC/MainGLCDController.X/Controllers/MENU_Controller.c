#include "MENU_Controller.h"
#include <string.h>

#define VALUE_NAME_LENGTH   3
#define VALUE_DATA_LENGTH   8
#define TITLE_NAME_LENGTH   6

#define V_SET_VALUE         0
#define I_SET_VALUE         1
#define V_READ_VALUE        2
#define I_READ_VALUE        3

/********************************************************************************
 *              DEFINITIONS
 *******************************************************************************/
#define ID_TITLE_LBL_1      0
#define ID_TITLE_LBL_2      1
#define ID_TITLE_LBL_3      2
#define ID_V_SET_LBL_1      3
#define ID_V_SET_LBL_2      4
#define ID_V_SET_LBL_3      5
#define ID_I_SET_LBL_1      6
#define ID_I_SET_LBL_2      7
#define ID_I_SET_LBL_3      8
#define ID_V_RD_LBL_1       9
#define ID_V_RD_LBL_2       10
#define ID_V_RD_LBL_3       11
#define ID_I_RD_LBL_1       12
#define ID_I_RD_LBL_2       13
#define ID_I_RD_LBL_3       14

/********************************************************************************
 *              PROTOTYPES
 *******************************************************************************/
void onButtonPressed(GLKButton_t button);

/********************************************************************************
 *              VARIABLES
 *******************************************************************************/

/********************************************************************************
 *              FUCTIONS
 *******************************************************************************/
void menuInit() {

    GLK_Init(&onButtonPressed);

    // Clear screen
    GLK_ClearScreen();
   
    // Dim Leds
    GLK_SetLEDIndicators(LTop, Off); 
    GLK_SetLEDIndicators(LMiddle, Off); 
    GLK_SetLEDIndicators(LBottom, Off); 
    
    // Draw contour lines
    GLK_DrawRectangle(1, 0, 0, 191, 63);
    GLK_DrawLine(1, 10, 191, 10);
    GLK_DrawLine(1, 36, 191, 36);
    GLK_DrawLine(63, 1, 63, 62);
    GLK_DrawLine(127, 1, 127, 62);
    GLK_DrawRoundedRectangle(1, 37, 62, 62, 3);
    GLK_DrawRoundedRectangle(64, 37, 126, 62, 3);
    GLK_DrawRoundedRectangle(128, 37, 190, 62, 3);

    // Setup labels
    GLK_InitializeLabel(ID_TITLE_LBL_1, 53,  1,   62,  9,   1, 0, 1, 0, 0); 
    GLK_InitializeLabel(ID_TITLE_LBL_2, 117, 1,   126, 9,   1, 0, 1, 0, 0); 
    GLK_InitializeLabel(ID_TITLE_LBL_3, 181, 1,   190, 9,   1, 0, 1, 0, 0); 

    GLK_InitializeLabel(ID_V_SET_LBL_1, 16,  11,  62,  22,  1, 0, 1, 0, 0); 
    GLK_InitializeLabel(ID_V_SET_LBL_2, 79,  11,  126, 22,  1, 0, 1, 0, 0); 
    GLK_InitializeLabel(ID_V_SET_LBL_3, 143, 11,  190, 22,  1, 0, 1, 0, 0); 
    
    GLK_InitializeLabel(ID_I_SET_LBL_1, 16,  23,  62,  34,  1, 0, 1, 0, 0); 
    GLK_InitializeLabel(ID_I_SET_LBL_2, 79,  23,  126, 34,  1, 0, 1, 0, 0); 
    GLK_InitializeLabel(ID_I_SET_LBL_3, 143, 23,  190, 34,  1, 0, 1, 0, 0); 

    GLK_InitializeLabel(ID_V_RD_LBL_1,  16,  38,  60,  48,  1, 0, 1, 0, 0); 
    GLK_InitializeLabel(ID_V_RD_LBL_2,  79,  38,  124, 48,  1, 0, 1, 0, 0); 
    GLK_InitializeLabel(ID_V_RD_LBL_3,  143, 38,  188, 48,  1, 0, 1, 0, 0); 

    GLK_InitializeLabel(ID_I_RD_LBL_1,  16,  49,  60,  61,  1, 0, 1, 0, 0); 
    GLK_InitializeLabel(ID_I_RD_LBL_2,  79,  49,  124, 61,  1, 0, 1, 0, 0); 
    GLK_InitializeLabel(ID_I_RD_LBL_3,  143, 49,  188, 61,  1, 0, 1, 0, 0); 

    // Setup static text
    GLK_WriteText(5, 2, "SUP 1");
    GLK_WriteText(68, 2, "SUP 2");
    GLK_WriteText(132, 2, "SUP 3");

    GLK_WriteText(2, 14, "V:");
    GLK_WriteText(65, 14, "V:");
    GLK_WriteText(129, 14, "V:");

    GLK_WriteText(2, 26, "I:");
    GLK_WriteText(65, 26, "I:");
    GLK_WriteText(129, 26, "I:");

    GLK_WriteText(5, 40, "V=");
    GLK_WriteText(68, 40, "V=");
    GLK_WriteText(132, 40, "V=");

    GLK_WriteText(5, 52, "I=");
    GLK_WriteText(68, 52, "I=");
    GLK_WriteText(132, 52, "I=");


    // Test
    GLK_UpdateLabel(ID_TITLE_LBL_1, "A");
    GLK_UpdateLabel(ID_TITLE_LBL_2, "B");
    GLK_UpdateLabel(ID_TITLE_LBL_3, "C");
    GLK_UpdateLabel(ID_V_SET_LBL_1, "  5,00V");
    GLK_UpdateLabel(ID_V_SET_LBL_2, " 12,00V");
    GLK_UpdateLabel(ID_V_SET_LBL_3, "  3,00V");
    GLK_UpdateLabel(ID_I_SET_LBL_1, " 0,10mA");
    GLK_UpdateLabel(ID_I_SET_LBL_2, " 0,20mA");
    GLK_UpdateLabel(ID_I_SET_LBL_3, " 0,30mA");
    GLK_UpdateLabel(ID_V_RD_LBL_1, "  5,00V");
    GLK_UpdateLabel(ID_V_RD_LBL_2, " 12,00V");
    GLK_UpdateLabel(ID_V_RD_LBL_3, "  3,00V");
    GLK_UpdateLabel(ID_I_RD_LBL_1, "  5,00V");
    GLK_UpdateLabel(ID_I_RD_LBL_2, " 12,00V");
    GLK_UpdateLabel(ID_I_RD_LBL_3, "  3,00V");
}

void menuSelect(const uint8_t menu, bool selected) {
    /* if (menus[menu].title.selected != selected) { */
        /* updateTitleSelection(&menus[menu].title, selected); */
    /* } */
}

void menuSetVoltageState(const uint8_t menu, const uint8_t state) {
    /* if (menus[menu].values[V_SET_VALUE].state != state) { */
        /* updateValueState(&menus[menu].values[V_SET_VALUE], state); */
    /* } */
}

void menuSetCurrentState(const uint8_t menu, const uint8_t state) {
    /* if (menus[menu].values[I_SET_VALUE].state != state) { */
     /* updateValueState(&menus[menu].values[I_SET_VALUE], state); */
    /* } */
}

void menuSetVoltageSet(const uint8_t menu, const char * v) {
    /* if (strcmp(menus[menu].values[V_SET_VALUE].value, v) != 0) { */
        /* updateValueValue(&menus[menu].values[V_SET_VALUE], v); */
    /* } */
}

void menuSetCurrentSet(const uint8_t menu, const char * i) {
     /* if (strcmp(menus[menu].values[I_SET_VALUE].value, i) != 0) { */
        /* updateValueValue(&menus[menu].values[I_SET_VALUE], i); */
    /* } */
}

void menuSetVoltageRead(const uint8_t menu, const char * v) {
     /* if (strcmp(menus[menu].values[V_READ_VALUE].value, v) != 0) { */
        /* updateValueValue(&menus[menu].values[V_READ_VALUE], v); */
    /* } */
}

void menuSetCurrentRead(const uint8_t menu, const char * i) {
     /* if (strcmp(menus[menu].values[I_READ_VALUE].value, i) != 0) { */
        /* updateValueValue(&menus[menu].values[I_READ_VALUE], i); */
    /* } */
}


/********************************************************************************
 *              PRIVATE FUCTIONS
 *******************************************************************************/
static void onButtonPressed(GLKButton_t button) {

}

