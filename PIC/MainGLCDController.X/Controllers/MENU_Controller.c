#include "MENU_Controller.h"
#include <string.h>

#define VALUE_NAME_LENGTH   3
#define VALUE_DATA_LENGTH   8
#define TITLE_NAME_LENGTH   6

#define V_SET_VALUE         0
#define I_SET_VALUE         1
#define V_READ_VALUE        2
#define I_READ_VALUE        3

// Helpers
typedef struct Pos {
    const uint8_t x;          // X location on screen
    const uint8_t y;          // Y location on screen
} Pos_t;

typedef struct Rect {
    const uint8_t x;          // X start position
    const uint8_t y;          // Y start position
    const uint8_t w;          // Width
    const uint8_t h;          // Height
} Rect_t;

typedef struct Value {
    const char name[3];       // Display name
    const Pos_t nPos;         // Name position
    char value[8];            // Display value
    const Pos_t vPos;         // Value position
    uint8_t state;            // State
    const Rect_t sRect;       // Selection rectangle
    bool selected;            // Selected
} Value_t;

typedef struct Title {
    const char name[6];       // Display name
    const Pos_t nPos;         // Name position
    char status;              // Status character
    const Pos_t sPos;         // Status position
    const Rect_t sRect;       // Selection rectangle
    bool selected;            // Selected
} Title_t;

typedef struct Menu {
    Title_t title;      // Menu title
    Value_t values[4];  // Values inside the menu
} Menu_t;

static void drawMenu(Menu_t * menu);
static void drawTitle(Title_t * title);
static void drawValue(Value_t * value);

static void updateTitleStatus(Title_t * title, char status);
static void updateTitleSelection(Title_t * title, bool selected);
static void updateValueValue(Value_t * value, const char * data);
static void updateValueState(Value_t * value, uint8_t state);


static Menu_t menu1 = {
    // Title
    { "Sup 1", { 16, 1 }, 'I', { 56, 1 }, { 0, 0, 63, 10 }, false },

    // Values
    {
        { "V:", { 4, 16 }, { 0 }, { 24, 16 }, STATE_NONE, { 1, 15, 12, 8 }, false  },
        { "I:", { 4, 26 }, { 0 }, { 24, 26 }, STATE_NONE, { 1, 25, 12, 8 }, false  },
        { "V=", { 5, 40 }, { 0 }, { 24, 40 }, STATE_NONE, { 1, 39, 12, 8 }, false  },
        { "I=", { 5, 50 }, { 0 }, { 24, 50 }, STATE_NONE, { 1, 49, 12, 8 }, false  },
    }
};

static Menu_t menu2 = {
    // Title
    { "Sup 2", { 80, 1 }, 'I', { 120, 1 }, { 64, 0, 63, 10 }, false },

    // Values
    {
        { "V:", { 68, 16 }, { 0 }, { 88, 16 }, STATE_NONE, { 65, 15, 12, 8 }, false  },
        { "I:", { 68, 26 }, { 0 }, { 88, 26 }, STATE_NONE, { 65, 25, 12, 8 }, false  },
        { "V=", { 69, 40 }, { 0 }, { 88, 40 }, STATE_NONE, { 65, 39, 12, 8 }, false  },
        { "I=", { 69, 50 }, { 0 }, { 88, 50 }, STATE_NONE, { 65, 49, 12, 8 }, false  },
    }
};

static Menu_t menu3 = {
    // Title
    { "Sup 3", { 144, 1 }, 'I', { 184, 1 }, { 128, 0, 63, 10 }, false },

    // Values
    {
        { "V:", { 132, 16 }, { 0 }, { 152, 16 }, STATE_NONE, { 129, 15, 12, 8 }, false  },
        { "I:", { 132, 26 }, { 0 }, { 152, 26 }, STATE_NONE, { 129, 25, 12, 8 }, false  },
        { "V=", { 133, 40 }, { 0 }, { 152, 40 }, STATE_NONE, { 129, 39, 12, 8 }, false  },
        { "I=", { 133, 50 }, { 0 }, { 152, 50 }, STATE_NONE, { 129, 49, 12, 8 }, false  },
    }
};

static Menu_t menus[3];

void menuInit() {

    GLK_Init();
    GLK_ClearScreen();
    
    GLK_Write("PANDA");


/*     // Initialize GLCD */
    /* GLCD_Init(NON_INVERTED); */
    /* GLCD_SelectFont(font); */
    /* GLCD_ClearScreen(BLACK); */

    /* menus[0] = menu1; */
    /* menus[1] = menu2; */
    /* menus[2] = menu3; */

    /* drawMenu(&menu1); */
    /* drawMenu(&menu2); */
    /* drawMenu(&menu3); */

    /* // Draw lines */
    /* GLCD_DrawHoriLine(0, 10, 191); */
    /* GLCD_DrawHoriLine(0, 36, 191); */
    /* GLCD_DrawHoriLine(0, 37, 191); */
    /* GLCD_DrawVertLine(64, 0, 63); */
    /* GLCD_DrawVertLine(128, 0, 63); */
    /* GLCD_DrawVertLine(0, 38, 26); */
    /* GLCD_DrawVertLine(65, 38, 26); */
    /* GLCD_DrawVertLine(129, 38, 26); */

}

void menuSelect(const uint8_t menu, bool selected) {
    if (menus[menu].title.selected != selected) {
        updateTitleSelection(&menus[menu].title, selected);
    }
}

void menuSetVoltageState(const uint8_t menu, const uint8_t state) {
    if (menus[menu].values[V_SET_VALUE].state != state) {
        updateValueState(&menus[menu].values[V_SET_VALUE], state);
    }
}

void menuSetCurrentState(const uint8_t menu, const uint8_t state) {
    if (menus[menu].values[I_SET_VALUE].state != state) {
        updateValueState(&menus[menu].values[I_SET_VALUE], state);
    }
}

void menuSetVoltageSet(const uint8_t menu, const char * v) {
    if (strcmp(menus[menu].values[V_SET_VALUE].value, v) != 0) {
        updateValueValue(&menus[menu].values[V_SET_VALUE], v);
    }
}

void menuSetCurrentSet(const uint8_t menu, const char * i) {
     if (strcmp(menus[menu].values[I_SET_VALUE].value, i) != 0) {
        updateValueValue(&menus[menu].values[I_SET_VALUE], i);
    }
}

void menuSetVoltageRead(const uint8_t menu, const char * v) {
     if (strcmp(menus[menu].values[V_READ_VALUE].value, v) != 0) {
        updateValueValue(&menus[menu].values[V_READ_VALUE], v);
    }
}

void menuSetCurrentRead(const uint8_t menu, const char * i) {
     if (strcmp(menus[menu].values[I_READ_VALUE].value, i) != 0) {
        updateValueValue(&menus[menu].values[I_READ_VALUE], i);
    }
}

static void drawMenu(Menu_t * menu) {
    drawTitle(&menu->title);
    drawValue(&menu->values[V_SET_VALUE]);
    drawValue(&menu->values[I_SET_VALUE]);
    drawValue(&menu->values[V_READ_VALUE]);
    drawValue(&menu->values[I_READ_VALUE]);
}

static void drawTitle(Title_t * title) {
    GLCD_WriteText(title->nPos.x, title->nPos.y, title->name);
    GLCD_GotoXY(title->sPos.x, title->sPos.y);
    GLCD_PutChar(title->status);
}

static void drawValue(Value_t * value) {
    GLCD_WriteText(value->nPos.x, value->nPos.y, value->name);
}

static void updateTitleStatus(Title_t * title, char status) {
    title->status = status;
    GLCD_GotoXY(title->sPos.x, title->sPos.y);
    GLCD_PutChar(title->status);
}

static void updateTitleSelection(Title_t * title, bool selected) {
    if (title->selected != selected) {
        GLCD_InvertRect(title->sRect.x, title->sRect.y, title->sRect.w, title->sRect.h);
        title->selected = selected;
    }
}

static void updateValueValue(Value_t * value, const char * data) {
    // Copy values into buffer
    strcpy(value->value, data);
    value->value[VALUE_DATA_LENGTH - 1] = 0; // Ensure termination
    GLCD_WriteText(value->vPos.x, value->vPos.y, value->value); 
}

static void updateValueState(Value_t * value, uint8_t state) {
    switch(state) {
        case STATE_NONE:
            GLCD_ClearDot(value->nPos.x - 2, value->nPos.y + 4);
            if (value->state == STATE_SELECT) {
                GLCD_InvertRect(value->sRect.x, value->sRect.y, value->sRect.w, value->sRect.h);
            }
            break;
        case STATE_POINT:
            GLCD_DrawDot(value->nPos.x - 2, value->nPos.y + 4);
            if (value->state == STATE_SELECT) {
                GLCD_InvertRect(value->sRect.x, value->sRect.y, value->sRect.w, value->sRect.h);
            }
            break;
        case STATE_SELECT:
            GLCD_DrawDot(value->nPos.x - 2, value->nPos.y + 4);
            if (value->state < STATE_SELECT) {
                GLCD_InvertRect(value->sRect.x, value->sRect.y, value->sRect.w, value->sRect.h);
            }
            break;

    }
    value->state = state;
}


