#include "MENU_Controller.h"

#define MENU_WIDTH      64
#define TITLE_HEIGHT    10
#define SETUP_HEIGHT    12
#define READ_HEIGHT     32

#define MENU_1_TITLE_START  (16)
#define MENU_2_TITLE_START  (16 + MENU_WIDTH)
#define MENU_3_TITLE_START  (16 + (2 * MENU_WIDTH))

#define MENU_1_SETUP_START  (8)
#define MENU_2_SETUP_START  (8 + MENU_WIDTH)
#define MENU_3_SETUP_START  (8 + (2 * MENU_WIDTH))

#define MENU_1_READ_START  (8)
#define MENU_2_READ_START  (8 + MENU_WIDTH)
#define MENU_3_READ_START  (8 + (2 * MENU_WIDTH))

// Helpers
typedef struct Pos {
    uint8_t x;          // X location on screen
    uint8_t y;          // Y location on screen
} Pos_t;

typedef struct Rect {
    uint8_t x;          // X start position
    uint8_t y;          // Y start position
    uint8_t w;          // Width
    uint8_t h;          // Height
} Rect_t;

typedef struct Value {
    char name[3];       // Display name
    Pos_t nPos;         // Name position
    char value[8];      // Display value
    Pos_t vPos;         // Value position
    uint8_t state;      // State
    Rect_t sRect;       // Selection rectangle
    bool selected;      // Selected
} Value_t;

typedef struct Title {
    char name[6];       // Display name
    Pos_t nPos;         // Name position
    char status;        // Status character
    Pos_t sPos;         // Status position
    Rect_t sRect;       // Selection rectangle
    bool selected;      // Selected
} Title_t;

typedef struct Menu {
    Title_t title;      // Menu title
    Value_t values[4];  // Values inside the menu
} Menu_t;

static void drawMenu(Menu_t * menu);
static void drawTitle(Title_t * title);
static void drawValue(Value_t * value);
static void updateMenu(Menu_t * menu);
static void updateTitle(Title_t * title);
static void updateValue(Value_t * value);

// Helpers
static bool menu_1_selected;
static bool menu_2_selected;
static bool menu_3_selected;

static bool menu_1_v_selected;
static bool menu_2_v_selected;
static bool menu_3_v_selected;

static bool menu_1_i_selected;
static bool menu_2_i_selected;
static bool menu_3_i_selected;


void menuInit() {

    // Initialize GLCD
    GLCD_Init(NON_INVERTED);
    GLCD_SelectFont(font);
    GLCD_ClearScreen(BLACK);

    // Print titles
    GLCD_WriteText(MENU_1_TITLE_START, 1, menu_1_title);
    GLCD_WriteText(MENU_2_TITLE_START, 1, menu_2_title);
    GLCD_WriteText(MENU_3_TITLE_START, 1, menu_3_title);
    
    GLCD_WriteText(MENU_1_SETUP_START, SETUP_HEIGHT, menu_setup);
    GLCD_WriteText(MENU_2_SETUP_START, SETUP_HEIGHT, menu_setup);
    GLCD_WriteText(MENU_3_SETUP_START, SETUP_HEIGHT, menu_setup);
    
    GLCD_WriteText(MENU_1_SETUP_START-2, SETUP_HEIGHT + 8,  menu_voltage_is);
    GLCD_WriteText(MENU_1_SETUP_START-2, SETUP_HEIGHT + 16, menu_current_is);
    GLCD_WriteText(MENU_2_SETUP_START-2, SETUP_HEIGHT + 8,  menu_voltage_is);
    GLCD_WriteText(MENU_2_SETUP_START-2, SETUP_HEIGHT + 16, menu_current_is);
    GLCD_WriteText(MENU_3_SETUP_START-2, SETUP_HEIGHT + 8,  menu_voltage_is);
    GLCD_WriteText(MENU_3_SETUP_START-2, SETUP_HEIGHT + 16, menu_current_is);
    
    GLCD_WriteText(MENU_1_READ_START-2, READ_HEIGHT + 8,  menu_voltage_read);
    GLCD_WriteText(MENU_1_READ_START-2, READ_HEIGHT + 16, menu_current_read);
    GLCD_WriteText(MENU_2_READ_START-2, READ_HEIGHT + 8,  menu_voltage_read);
    GLCD_WriteText(MENU_2_READ_START-2, READ_HEIGHT + 16, menu_current_read);
    GLCD_WriteText(MENU_3_READ_START-2, READ_HEIGHT + 8,  menu_voltage_read);
    GLCD_WriteText(MENU_3_READ_START-2, READ_HEIGHT + 16, menu_current_read);
    
    // Draw lines
    GLCD_DrawHoriLine(0, TITLE_HEIGHT, DISPLAY_WIDTH - 1);
    GLCD_DrawVertLine(MENU_WIDTH, 0, DISPLAY_HEIGHT - 1,);
    GLCD_DrawVertLine(2 * MENU_WIDTH, 0, DISPLAY_HEIGHT - 1);

    // Test
    menuSelect(MENU_1);
    menuPointVoltage(MENU_1);
    menuPointVoltage(MENU_2);
    menuPointCurrent(MENU_3);
    menuPointCurrent(MENU_1);
}

void menuSelect(const uint8_t menu) {
    switch (menu) {
        default:
        case MENU_1:
            if (!menu_1_selected) {
                GLCD_InvertRect(0, 0, MENU_WIDTH - 1, TITLE_HEIGHT - 1);
                menu_1_selected = true;
            }
            break;
            
        case MENU_2:
            if (!menu_2_selected) {
                GLCD_InvertRect(MENU_WIDTH, 0, MENU_WIDTH - 1, TITLE_HEIGHT - 1);
                menu_2_selected = true;
            }
            break;
            
        case MENU_3:
            if (!menu_3_selected) {
                GLCD_InvertRect(2*MENU_WIDTH, 0, MENU_WIDTH - 1, TITLE_HEIGHT - 1);
                menu_3_selected = true;
            }
            break;
    }
}

void menuDeselect(const uint8_t menu) {
    switch (menu) {
        default:
        case MENU_1:
            if (menu_1_selected) {
                GLCD_InvertRect(0, 0, MENU_WIDTH - 1, TITLE_HEIGHT - 1);
                menu_1_selected = false;
            }
            break;
            
        case MENU_2:
            if (menu_2_selected) {
                GLCD_InvertRect(MENU_WIDTH, 0, MENU_WIDTH - 1, TITLE_HEIGHT - 1);
                menu_2_selected = false;
            }
            break;
            
        case MENU_3:
            if (menu_3_selected) {
                GLCD_InvertRect(2*MENU_WIDTH, 0, MENU_WIDTH - 1, TITLE_HEIGHT - 1);
                menu_3_selected = false;
            }
            break;
    }
}

void menuPointVoltage(const uint8_t menu) {
    switch (menu) {
        default:
        case MENU_1:
            GLCD_ClearDot(MENU_1_SETUP_START-4, SETUP_HEIGHT + 20)
            GLCD_DrawDot(MENU_1_SETUP_START-4, SETUP_HEIGHT + 12)
            break;
            
        case MENU_2:
            GLCD_ClearDot(MENU_2_SETUP_START-4, SETUP_HEIGHT + 20)
            GLCD_DrawDot(MENU_2_SETUP_START-4, SETUP_HEIGHT + 12)
            break;
            
        case MENU_3:
            GLCD_ClearDot(MENU_3_SETUP_START-4, SETUP_HEIGHT + 20)
            GLCD_DrawDot(MENU_3_SETUP_START-4, SETUP_HEIGHT + 12)
            break;
    }
}

void menuPointCurrent(const uint8_t menu) {
    switch (menu) {
        default:
        case MENU_1:
            GLCD_ClearDot(MENU_1_SETUP_START-4, SETUP_HEIGHT + 12)
            GLCD_DrawDot(MENU_1_SETUP_START-4, SETUP_HEIGHT + 20)
            break;
            
        case MENU_2:
            GLCD_ClearDot(MENU_2_SETUP_START-4, SETUP_HEIGHT + 12)
            GLCD_DrawDot(MENU_2_SETUP_START-4, SETUP_HEIGHT + 20)
            break;
            
        case MENU_3:
            GLCD_ClearDot(MENU_3_SETUP_START-4, SETUP_HEIGHT + 12)
            GLCD_DrawDot(MENU_3_SETUP_START-4, SETUP_HEIGHT + 20)
            break;
    }
}

void menuSelectVoltage(const uint8_t menu) {
    menuPointVoltage(menu);
    switch (menu) {
        default:
        case MENU_1:
            if (!menu_1_v_selected) {
                if (menu_1_i_selected) {
                    GLCD_InvertRect(MENU_1_SETUP_START - 5, SETUP_HEIGHT + 15, 14, 8); // Invert current
                }
                GLCD_InvertRect(MENU_1_SETUP_START - 5, SETUP_HEIGHT + 8, 14, 8); // Invert voltage
                menu_1_v_selected = true;
            }
            break;
            
        case MENU_2:
            if (!menu_2_v_selected) {
                if (menu_2_i_selected) {
                    GLCD_InvertRect(MENU_2_SETUP_START - 5, SETUP_HEIGHT + 15, 14, 8); // Invert current
                }
                GLCD_InvertRect(MENU_2_SETUP_START - 5, SETUP_HEIGHT + 8, 14, 8); // Invert voltage
                menu_2_v_selected = true;
            }
            break;
            
        case MENU_3:
            if (!menu_3_v_selected) {
                if (menu_3_i_selected) {
                    GLCD_InvertRect(MENU_3_SETUP_START - 5, SETUP_HEIGHT + 15, 14, 8); // Invert current
                }
                GLCD_InvertRect(MENU_3_SETUP_START - 5, SETUP_HEIGHT + 8, 14, 8); // Invert voltage
                menu_3_v_selected = true;
            }
            break;
    }
}

void menuSelectCurrent(const uint8_t menu) {
    menuPointCurrent(menu);
     switch (menu) {
        default:
        case MENU_1:
            if (!menu_1_i_selected) {
                if (menu_1_v_selected) {
                    GLCD_InvertRect(MENU_1_SETUP_START - 5, SETUP_HEIGHT + 8, 14, 8); // Invert current
                }
                GLCD_InvertRect(MENU_1_SETUP_START - 5, SETUP_HEIGHT + 15, 14, 8); // Invert voltage
                menu_1_i_selected = true;
            }
            break;
            
        case MENU_2:
            if (!menu_2_i_selected) {
                if (menu_2_v_selected) {
                    GLCD_InvertRect(MENU_2_SETUP_START - 5, SETUP_HEIGHT + 8, 14, 8); // Invert current
                }
                GLCD_InvertRect(MENU_2_SETUP_START - 5, SETUP_HEIGHT + 15, 14, 8); // Invert voltage
                menu_2_i_selected = true;
            }
            break;
            
        case MENU_3:
            if (!menu_3_i_selected) {
                if (menu_3_v_selected) {
                    GLCD_InvertRect(MENU_3_SETUP_START - 5, SETUP_HEIGHT + 8, 14, 8); // Invert current
                }
                GLCD_InvertRect(MENU_3_SETUP_START - 5, SETUP_HEIGHT + 15, 14, 8); // Invert voltage
                menu_3_i_selected = true;
            }
            break;
    }
 }

void menuSetVoltageSet(const uint8_t menu, const char * v) {
    switch (menu) {
        default:
        case MENU_1:
            GLCD_WriteText(MENU_1_SETUP_START + 11, SETUP_HEIGHT + 8, v);
            break;
            
        case MENU_2:
            GLCD_WriteText(MENU_2_SETUP_START + 11, SETUP_HEIGHT + 8, v);
            break;
            
        case MENU_3:
            GLCD_WriteText(MENU_3_SETUP_START + 11, SETUP_HEIGHT + 8, v);
            break;
    }
}

void menuSetCurrentSet(const uint8_t menu, const char * i) {
    switch (menu) {
        default:
        case MENU_1:
            GLCD_WriteText(MENU_1_SETUP_START + 11, SETUP_HEIGHT + 16, i);
            break;
            
        case MENU_2:
            GLCD_WriteText(MENU_2_SETUP_START + 11, SETUP_HEIGHT + 16, i);
            break;
            
        case MENU_3:
            GLCD_WriteText(MENU_3_SETUP_START + 11, SETUP_HEIGHT + 16, i);
            break;
    }
}

void menuSetVoltageRead(const uint8_t menu, const char * v) {
    switch (menu) {
        default:
        case MENU_1:
            GLCD_WriteText(MENU_1_READ_START + 11, READ_HEIGHT + 8, v);
            break;
            
        case MENU_2:
            GLCD_WriteText(MENU_2_READ_START + 11, READ_HEIGHT + 8, v);
            break;
            
        case MENU_3:
            GLCD_WriteText(MENU_3_READ_START + 11, READ_HEIGHT + 8, v);
            break;
    }
}

void menuSetCurrentRead(const uint8_t menu, const char * i) {
    switch (menu) {
        default:
        case MENU_1:
            GLCD_WriteText(MENU_1_READ_START + 11, READ_HEIGHT + 16, i);
            break;
            
        case MENU_2:
            GLCD_WriteText(MENU_2_READ_START + 11, READ_HEIGHT + 16, i);
            break;
            
        case MENU_3:
            GLCD_WriteText(MENU_3_READ_START + 11, READ_HEIGHT + 16, i);
            break;
    }
}


static void drawMenu(Menu_t * menu) {
    drawTitle(&menu->title);
    drawValue(&menu->values[0]);
    drawValue(&menu->values[1]);
    drawValue(&menu->values[2]);
    drawValue(&menu->values[3]);
}

static void drawTitle(Title_t * title) {
    GLCD_WriteText(title->nPos.x, title->nPos.y, title->name);
    GLCD_GotoXY(title->sPos.x, title->sPos.y);
    GLCD_PutChar(title->status);
}

static void drawValue(Value_t * value) {
    GLCD_WriteText(value->nPos.x, value->nPos.y, value->name);
}

static void updateMenu(Menu_t * menu) {

}

static void updateTitle(Title_t * title) {

}

static void updateValue(Value_t * value) { 

}


