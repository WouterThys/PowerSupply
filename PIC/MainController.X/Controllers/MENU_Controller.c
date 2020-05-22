#include "MENU_Controller.h"
#include "../Utils.h"
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
#define ID_TITLE_LBL      0
#define ID_V_SET_LBL      1
#define ID_I_SET_LBL      2
#define ID_T_SET_LBL      3
#define ID_V_RD_LBL       4
#define ID_I_RD_LBL       5
#define ID_T_RD_LBL       6

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
static int8_t menu_id = -1;
static bool connected = false;
static bool enabled = false;
static bool pid = false;
static uint16_t set_voltage = 0;
static uint16_t msr_voltage = 0;
static uint16_t set_current = 0;
static uint16_t msr_current = 0;
static uint16_t set_temp = 0;
static uint16_t msr_temp = 0;
static SupplySelection_e selection = Temperature;


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
    GLK_DrawRectangle(1, 0, 0, 191, 63); // Around screen
    GLK_DrawLine(1,   10, 191, 10); // Title separator 1
    GLK_DrawLine(1,   11, 191, 11); // Title separator 2
    GLK_DrawLine(64,  21, 191, 21); // Menu separator 1
    GLK_DrawLine(126, 11, 126, 62); // Menu separator 2
    
    GLK_WriteText(2,   2,  "SUPPLY: ");
    GLK_WriteText(80,  13, "SET");
    GLK_WriteText(146, 13, "MSR");
    
    GLK_WriteText(70,  2,  "CON");
    GLK_WriteText(110, 2,  "EN");
    GLK_WriteText(140, 2,  "PID");
    
    GLK_WriteText(10,  26, "Voltage:");
    GLK_WriteText(10,  38, "Current:");
    GLK_WriteText(10,  50, "Temp.  :");
    
    GLK_WriteText(115, 26, "V");
    GLK_WriteText(182, 26, "V");
    GLK_WriteText(111, 38, "mA");
    GLK_WriteText(178, 38, "mA");
    GLK_WriteText(117, 50, "C");
    GLK_WriteText(184, 50, "C");
    
    GLK_DrawCircle(110,52, 1);
    GLK_DrawCircle(180,52, 1);
    
    GLK_DrawBitmapFromMemory(CHECK_FALSE_ID, 90,  1);   // Connected
    GLK_DrawBitmapFromMemory(CHECK_FALSE_ID, 124, 1);   // Enabled
    GLK_DrawBitmapFromMemory(CHECK_FALSE_ID, 160, 1);   // PID
    
    // Setup labels
    GLK_InitializeLabel(ID_TITLE_LBL, 50,  1,   57,   9,   1, 0, 1, 0, 0);
    GLK_InitializeLabel(ID_V_SET_LBL, 70,  25,  109,  35,  1, 2, 1, 0, 0); 
    GLK_InitializeLabel(ID_I_SET_LBL, 70,  37,  109,  47,  1, 2, 1, 0, 0); 
    GLK_InitializeLabel(ID_V_RD_LBL, 127,  25,  176,  35,  1, 2, 1, 0, 0); 
    GLK_InitializeLabel(ID_I_RD_LBL, 127,  37,  176,  47,  1, 2, 1, 0, 0); 
    
    GLK_DrawBitmapFromMemory(NO_ARROW_ID, 2, 27);
    GLK_DrawBitmapFromMemory(NO_ARROW_ID, 2, 39);
    GLK_DrawBitmapFromMemory(NO_ARROW_ID, 2, 51);
}

void menuSetId(uint8_t id) {
    if (menu_id != id) {
        menu_id = id;
        const char data[] = { id + 48, 0 };
        GLK_UpdateLabel(ID_TITLE_LBL, &data[0]);
    }
}

void menuSetConnected(bool c) {
    if (connected != c) {
        connected = c;
        if (connected) {
             GLK_DrawBitmapFromMemory(CHECK_TRUE_ID, 90,  1);
        } else {
             GLK_DrawBitmapFromMemory(CHECK_FALSE_ID, 90,  1);
        }
    }
}

void menuSetEnabled(bool e) {
    if (enabled != e) {
        enabled = e;
        if (enabled) {
            GLK_DrawBitmapFromMemory(CHECK_TRUE_ID, 124, 1);
        } else {
            GLK_DrawBitmapFromMemory(CHECK_FALSE_ID, 124, 1);
        }
    }
}

void menuSetPidEnabled(bool e) {
    if (pid != e) {
        pid = e;
        if (pid) {
            GLK_DrawBitmapFromMemory(CHECK_TRUE_ID, 160, 1);
        } else {
            GLK_DrawBitmapFromMemory(CHECK_FALSE_ID, 160, 1);
        }
    }
}

void menuSetSelection(SupplySelection_e s) {
    if (s != selection) {
        
        switch (selection) {
            case Voltage:
                GLK_DrawBitmapFromMemory(NO_ARROW_ID, 2, 27);
                break;
            case Current:
                GLK_DrawBitmapFromMemory(NO_ARROW_ID, 2, 39);
                break;
            case Temperature:
                GLK_DrawBitmapFromMemory(NO_ARROW_ID, 2, 51);
                break;
        }
        
        selection = s;
        switch (selection) {
            case Voltage:
                GLK_DrawBitmapFromMemory(ARROW_ID, 2, 27);
                break;
            case Current:
                GLK_DrawBitmapFromMemory(ARROW_ID, 2, 39);
                break;
            case Temperature:
                GLK_DrawBitmapFromMemory(ARROW_ID, 2, 51);
                break;
        }
        
    }
}

void menuSetVoltageSet(uint16_t value) {
    
    // Check
    if (value == set_voltage) return;
    set_voltage = value;
    
        // Result value:    d1   d2        d3   d4   
    char result[6] = { ' ', '5', '.', '0', '0', 0};
    
    uint16_t d1 = value / 10000; value %= 10000;
    uint16_t d2 = value / 1000; value %= 1000;
    uint16_t d3 = value / 100; value %= 100;
    uint16_t d4 = value / 10; 
    
    result[0] = d1 > 0 ? (d1 + 48) : ' '; 
    result[1] = d2 + 48;
    result[3] = d3 + 48;
    result[4] = d4 + 48;
    
    GLK_UpdateLabel(ID_V_SET_LBL, result);
}

void menuSetCurrentSet(uint16_t value) {
    
    // Check
    if (value == set_current) return;
    set_current = value;
    
        // Result value:    d1   d2    d3   d4   
    char result[6] = { ' ', '1', '0', '0', '0', 0};
    
    uint16_t d0 = value / 10000; value %= 10000;
    uint16_t d1 = value / 1000; value %= 1000;
    uint16_t d2 = value / 100; value %= 100;
    uint16_t d3 = value / 10; value %= 10;
    uint16_t d4 = value / 1; 
    
    result[0] = (d0 > 0) ? (d0 + 48) : ' '; 
    result[1] = (d0 > 0 || d1 > 0) ? (d1 + 48) : ' '; 
    result[2] = (d0 > 0 || d1 > 0 || d2 > 0) ? (d2 + 48) : ' ';
    result[3] = (d0 > 0 || d1 > 0 || d2 > 0 || d3 > 0) ? (d3 + 48) : ' ';
    result[4] = d4 + 48;
    
    GLK_UpdateLabel(ID_I_SET_LBL, result);    
}

void menuSetTemperatureSet(uint16_t value) {
    
    // Check
    if (value == set_temp) return;
    set_temp = value;
}

void menuSetVoltageMsr(uint16_t v) {
    
    // Check
    if (v == msr_voltage) return;
    msr_voltage = v;
    
    uint16_t value = digitalToVoltage(v);
    
    // Result value:    d1   d2        d3   d4   
    char result[6] = { ' ', '5', '.', '0', '0', 0};
    
    uint16_t d1 = value / 10000; value %= 10000;
    uint16_t d2 = value / 1000; value %= 1000;
    uint16_t d3 = value / 100; value %= 100;
    uint16_t d4 = value / 10; 
    
    result[0] = d1 > 0 ? (d1 + 48) : ' '; 
    result[1] = d2 + 48;
    result[3] = d3 + 48;
    result[4] = d4 + 48;
    
    GLK_UpdateLabel(ID_V_RD_LBL, result);
}

void menuSetCurrentMsr(uint16_t i) {
    
    // Check
    if (i == msr_current) return;
    msr_current = i;
    
     uint16_t value = digitalToCurrent(i);
    
        // Result value:    d1   d2    d3   d4   
    char result[6] = { ' ', '1', '0', '0', '0', 0};
    
    uint16_t d0 = value / 10000; value %= 10000;
    uint16_t d1 = value / 1000; value %= 1000;
    uint16_t d2 = value / 100; value %= 100;
    uint16_t d3 = value / 10; value %= 10;
    uint16_t d4 = value / 1; 
    
    result[0] = (d0 > 0) ? (d0 + 48) : ' '; 
    result[1] = (d0 > 0 || d1 > 0) ? (d1 + 48) : ' '; 
    result[2] = (d0 > 0 || d1 > 0 || d2 > 0) ? (d2 + 48) : ' ';
    result[3] = (d0 > 0 || d1 > 0 || d2 > 0 || d3 > 0) ? (d3 + 48) : ' ';
    result[4] = d4 + 48;
    
    GLK_UpdateLabel(ID_I_RD_LBL, result);    
}

void menuSetTemperatureMsr(uint16_t value) {
    
    // Check
    if (value == msr_temp) return;
    msr_temp = value;
    
}

//
//
//
//
//void menuSetVoltageState(const uint8_t menu, const uint8_t state) {
////    menuSelect(menu);
////    GLK_SetDrawingColor(state);
////    GLK_DrawFilledCircle(
////        menu_select_v_locs[menu][0],
////        menu_select_v_locs[menu][1],
////        1);
//}
//
//void menuSetCurrentState(const uint8_t menu, const uint8_t state) {
////    menuSelect(menu);
////    GLK_SetDrawingColor(state);
////    GLK_DrawFilledCircle(
////        menu_select_i_locs[menu][0],
////        menu_select_i_locs[menu][1],
////        1);
//}
//
//void menuSetVoltageSet(const uint8_t menu, uint16_t value) {    
//    // Result value:    d1   d2        d3   d4   
//    char result[6] = { ' ', '5', '.', '0', '0', 0};
//    
//    uint16_t d1 = value / 10000; value %= 10000;
//    uint16_t d2 = value / 1000; value %= 1000;
//    uint16_t d3 = value / 100; value %= 100;
//    uint16_t d4 = value / 10; 
//    
//    result[0] = d1 > 0 ? (d1 + 48) : ' '; 
//    result[1] = d2 + 48;
//    result[3] = d3 + 48;
//    result[4] = d4 + 48;
//    
//    GLK_UpdateLabel(ID_V_SET_LBL, result);
//}
//
//void menuSetCurrentSet(const uint8_t menu, uint16_t value) {    
//        // Result value:    d1   d2    d3   d4   
//    char result[6] = { ' ', '1', '0', '0', '0', 0};
//    
//    uint16_t d0 = value / 10000; value %= 10000;
//    uint16_t d1 = value / 1000; value %= 1000;
//    uint16_t d2 = value / 100; value %= 100;
//    uint16_t d3 = value / 10; value %= 10;
//    uint16_t d4 = value / 1; 
//    
//    result[0] = (d0 > 0) ? (d0 + 48) : ' '; 
//    result[1] = (d0 > 0 || d1 > 0) ? (d1 + 48) : ' '; 
//    result[2] = (d0 > 0 || d1 > 0 || d2 > 0) ? (d2 + 48) : ' ';
//    result[3] = (d0 > 0 || d1 > 0 || d2 > 0 || d3 > 0) ? (d3 + 48) : ' ';
//    result[4] = d4 + 48;
//    
//    GLK_UpdateLabel(ID_I_SET_LBL, result);
//}
//
//void menuSetVoltageRead(const uint8_t menu, uint16_t value) {
//    // TODO
//    if (menu != 0) return;
//    
//    // Result value:    d1   d2        d3   d4   
//    char result[6] = { ' ', '5', '.', '0', '0', 0};
//    
//    uint16_t d1 = value / 10000; value %= 10000;
//    uint16_t d2 = value / 1000; value %= 1000;
//    uint16_t d3 = value / 100; value %= 100;
//    uint16_t d4 = value / 10; 
//    
//    result[0] = d1 > 0 ? (d1 + 48) : ' '; 
//    result[1] = d2 + 48;
//    result[3] = d3 + 48;
//    result[4] = d4 + 48;
//    
//    GLK_UpdateLabel(ID_V_RD_LBL, result);
//}
//
//void menuSetCurrentRead(const uint8_t menu, uint16_t value) {
//    
//        // Result value:    d1   d2    d3   d4   
//    char result[6] = { ' ', '1', '0', '0', '0', 0};
//    
//    uint16_t d0 = value / 10000; value %= 10000;
//    uint16_t d1 = value / 1000; value %= 1000;
//    uint16_t d2 = value / 100; value %= 100;
//    uint16_t d3 = value / 10; value %= 10;
//    uint16_t d4 = value / 1; 
//    
//    result[0] = (d0 > 0) ? (d0 + 48) : ' '; 
//    result[1] = (d0 > 0 || d1 > 0) ? (d1 + 48) : ' '; 
//    result[2] = (d0 > 0 || d1 > 0 || d2 > 0) ? (d2 + 48) : ' ';
//    result[3] = (d0 > 0 || d1 > 0 || d2 > 0 || d3 > 0) ? (d3 + 48) : ' ';
//    result[4] = d4 + 48;
//    
//    GLK_UpdateLabel(ID_I_RD_LBL, result);
//}
