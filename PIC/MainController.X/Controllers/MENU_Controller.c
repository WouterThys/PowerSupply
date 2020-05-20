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

#define SELECT_MENU_1_X     5
#define SELECT_MENU_1_Y     5
#define SELECT_MENU_2_X     68
#define SELECT_MENU_2_Y     5
#define SELECT_MENU_3_X     132
#define SELECT_MENU_3_Y     5

/********************************************************************************
 *              PROTOTYPES
 *******************************************************************************/

/********************************************************************************
 *              VARIABLES
 *******************************************************************************/
static int8_t selected_menu = -1;
static uint8_t menu_select_locs[3][2] = {
    { 5, 5 },
    { 68, 5 },
    { 132, 5 }
};
static uint8_t menu_select_v_locs[3][2] = {
    { 2, 17 },
    { 65, 17 },
    { 129, 17 }
};
static uint8_t menu_select_i_locs[3][2] = {
    { 2, 29 },
    { 65, 29 },
    { 129, 29 }
};
static uint8_t menu_voltage_ids[3] = {
    ID_V_SET_LBL_1,
    ID_V_SET_LBL_2,
    ID_V_SET_LBL_3,
};
static uint8_t menu_current_ids[3] = {
    ID_I_SET_LBL_1,
    ID_I_SET_LBL_2,
    ID_I_SET_LBL_3,
};
static uint8_t menu_voltage_rd_ids[3] = {
    ID_V_RD_LBL_1,
    ID_V_RD_LBL_2,
    ID_V_RD_LBL_3,
};
static uint8_t menu_current_rd_ids[3] = {
    ID_I_RD_LBL_1,
    ID_I_RD_LBL_2,
    ID_I_RD_LBL_3,
};

/******************************************************************************** *              
 *                      FUCTIONS
 *******************************************************************************/
void menuInit(buttonCallback callback) {

    GLK_Init(callback);

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
    GLK_WriteText(12, 2, "SUP 1");
    GLK_WriteText(75, 2, "SUP 2");
    GLK_WriteText(140, 2, "SUP 3");

    GLK_WriteText(4, 14, "V:");
    GLK_WriteText(67, 14, "V:");
    GLK_WriteText(131, 14, "V:");
//    GLK_WriteText(57, 14, "V");
//    GLK_WriteText(121, 14, "V");
//    GLK_WriteText(184, 14, "V");

    GLK_WriteText(4, 26, "I:");
    GLK_WriteText(67, 26, "I:");
    GLK_WriteText(131, 26, "I:");

    GLK_WriteText(5, 40, "V=");
    GLK_WriteText(68, 40, "V=");
    GLK_WriteText(132, 40, "V=");

    GLK_WriteText(5, 52, "I=");
    GLK_WriteText(68, 52, "I=");
    GLK_WriteText(132, 52, "I=");

    menuSelect(1);
}

void menuSelect(const int8_t menu) {
    if (menu != selected_menu) {
        // Unselect previous menu
        if (selected_menu >= 0) {
            GLK_SetDrawingColor(0);
            GLK_DrawFilledCircle(
                    menu_select_locs[selected_menu][0],
                    menu_select_locs[selected_menu][1],
                    3);
        }

        // Select new menu
        selected_menu = menu;
        GLK_SetDrawingColor(1);
        GLK_DrawFilledCircle(
                menu_select_locs[selected_menu][0],
                menu_select_locs[selected_menu][1],
                3);
    }
}

void menuSetVoltageState(const uint8_t menu, const uint8_t state) {
    menuSelect(menu);
    GLK_SetDrawingColor(state);
    GLK_DrawFilledCircle(
        menu_select_v_locs[menu][0],
        menu_select_v_locs[menu][1],
        1);
}

void menuSetCurrentState(const uint8_t menu, const uint8_t state) {
    menuSelect(menu);
    GLK_SetDrawingColor(state);
    GLK_DrawFilledCircle(
        menu_select_i_locs[menu][0],
        menu_select_i_locs[menu][1],
        1);
}

void menuSetVoltageSet(const uint8_t menu, uint16_t value) {
    // Result value:    d1   d2        d3   d4   x   
    char result[8] = { ' ', '5', '.', '0', '0', ' ', 'V', 0};
    
    uint16_t d1 = value / 10000; value %= 10000;
    uint16_t d2 = value / 1000; value %= 1000;
    uint16_t d3 = value / 100; value %= 100;
    uint16_t d4 = value / 10; 
    
    result[0] = d1 > 0 ? (d1 + 48) : ' '; 
    result[1] = d2 + 48;
    result[3] = d3 + 48;
    result[4] = d4 + 48;
    
    GLK_UpdateLabel(menu_voltage_ids[menu], result);
}

void menuSetCurrentSet(const uint8_t menu, uint16_t value) {
    // Result value:    d1   d2        d3   d4   x   
    char result[8] = { ' ', '5', '.', '0', '0', 'm', 'A', 0};
    
    uint16_t d1 = value / 10000; value %= 10000;
    uint16_t d2 = value / 1000; value %= 1000;
    uint16_t d3 = value / 100; value %= 100;
    uint16_t d4 = value / 10; 
    
    result[0] = d1 > 0 ? (d1 + 48) : ' '; 
    result[1] = d2 + 48;
    result[3] = d3 + 48;
    result[4] = d4 + 48;
    
    GLK_UpdateLabel(menu_current_ids[menu], result);
}

void menuSetVoltageRead(const uint8_t menu, uint16_t value) {
    // Result value:    d1   d2        d3   d4   x   
    char result[8] = { ' ', '5', '.', '0', '0', ' ', 'V', 0};
    
    uint16_t d1 = value / 10000; value %= 10000;
    uint16_t d2 = value / 1000; value %= 1000;
    uint16_t d3 = value / 100; value %= 100;
    uint16_t d4 = value / 10; 
    
    result[0] = d1 > 0 ? (d1 + 48) : ' '; 
    result[1] = d2 + 48;
    result[3] = d3 + 48;
    result[4] = d4 + 48;
    
    GLK_UpdateLabel(menu_voltage_rd_ids[menu], result);
}

void menuSetCurrentRead(const uint8_t menu, uint16_t value) {
        // Result value:    d1   d2        d3   d4   x   
    char result[8] = { ' ', '5', '.', '0', '0', 'm', 'A', 0};
    
    uint16_t d1 = value / 10000; value %= 10000;
    uint16_t d2 = value / 1000; value %= 1000;
    uint16_t d3 = value / 100; value %= 100;
    uint16_t d4 = value / 10; 
    
    result[0] = d1 > 0 ? (d1 + 48) : ' '; 
    result[1] = d2 + 48;
    result[3] = d3 + 48;
    result[4] = d4 + 48;
    
    GLK_UpdateLabel(menu_current_rd_ids[menu], result);
}
