#include "GLK19264A_Driver.h"


/********************************************************************************
 *              COMMANDS 
 *******************************************************************************/

// TEXT
static const uint8_t CLEAR_SCREEN[2]        = {0xFE, 0x58};
static const uint8_t GO_HOME[2]             = {0xFE, 0x48};
static const uint8_t SET_CURSOR_POS[2]      = {0xFE, 0x47};
static const uint8_t SET_CURSOR_COORD[2]    = {0xFE, 0x79};
static const uint8_t GET_STRING_EXTENDS[2]  = {0xFE, 0x29};
static const uint8_t INIT_TEXT_WINDOW[2]    = {0xFE, 0x2B};
static const uint8_t SET_TEXT_WINDOW[2]     = {0xFE, 0x2A};
static const uint8_t CLEAR_TEXT_WINDOW[2]   = {0xFE, 0x2C};
static const uint8_t INIT_LABEL[2]          = {0xFE, 0x2D};
static const uint8_t INIT_SCROLLING_LABEL[2]= {0xFE, 0x2F};
static const uint8_t UPDATE_LABEL[2]        = {0xFE, 0x2E};
static const uint8_t AUTO_SCROLL_ON[2]      = {0xFE, 0x51};
static const uint8_t AUTO_SCROLL_OFF[2]     = {0xFE, 0x52};

// DAWING
static const uint8_t SET_DRAWING_COLOR[2]   = {0xFE, 0x63};
static const uint8_t DRAW_PIXEL[2]          = {0xFE, 0x70};
static const uint8_t DRAW_LINE[2]           = {0xFE, 0x6C};
static const uint8_t CONTINUE_LINE[2]       = {0xFE, 0x65};
static const uint8_t DRAW_RECTANGLE[2]      = {0xFE, 0x72};
static const uint8_t DRAW_FILLED_RECT[2]    = {0xFE, 0x78};
static const uint8_t DRAW_ROUNDED_RECT[2]   = {0xFE, 0x80};

// GPIO
static const uint8_t GPO_ON[2]              = {0xFE, 0x57};
static const uint8_t GPO_OFF[2]             = {0xFE, 0x56};
static const uint8_t SET_START_UP_STATE[2]  = {0xFE, 0xC3};

// LED Indicators
static const uint8_t SET_LED_INDICATORS[2]  = {0xFE, 0x5A};

// MISC
static const uint8_t READ_VERSION_NUMBER[2] = {0xFE, 0x36};
static const uint8_t READ_MODULE_NUMBER[2]  = {0xFE, 0x37};

/********************************************************************************
 *              PROTOTYPES 
 *******************************************************************************/
static void uartDataRead(uint8_t data);
static void write(const uint8_t * command, const uint8_t length);
static buttonCallback btnCallback;

/********************************************************************************
 *              INTERNAL FUNCTIONS 
 *******************************************************************************/

static void uartDataRead(uint8_t data) {
    btnCallback((GLKButton_t)data);
    
}

static void write(const uint8_t * command, const uint8_t length) {
    uint8_t i = 0;
    for (i = 0; i < length; i++) {
        uartDriverWriteByte(command[i]);
    }
}

/********************************************************************************
 *              DRIVER FUNCTIONS
 *******************************************************************************/

void GLK_Init(buttonCallback callback) {
    // Callback
    btnCallback = callback;

    // Initialize UART
    uartDriverInit(UART1_BAUD, uartDataRead);   
    uartDriverEnable(true);
}

void GLK_WriteText(uint8_t x, uint8_t y, const char * text) {

    // Go to positions
    GLK_SetCursorCoordinate(x, y);
    
    char c = text[0];
    uint8_t i = 0;

    while (c != 0) {
        uartDriverWriteByte(c);
        i++;
        c = text[i];
    }
}

void GLK_ClearScreen() {
    write(CLEAR_SCREEN, 2);
}

void GLK_GoHome() {
    write(GO_HOME, 2);
}

void GLK_SetCursorPosition(uint8_t c, uint8_t r) {
    uint8_t data[4];
    data[0] = SET_CURSOR_POS[0];
    data[1] = SET_CURSOR_POS[1];
    data[2] = c;
    data[3] = r;
    write(data, 4);
}

void GLK_SetCursorCoordinate(uint8_t x, uint8_t y) {
    uint8_t data[4];
    data[0] = SET_CURSOR_COORD[0];
    data[1] = SET_CURSOR_COORD[1];
    data[2] = x;
    data[3] = y;
    write(data, 4);
}

uint8_t * GLK_GetStringExtends(const char * text) {

    uartDriverWriteByte(GET_STRING_EXTENDS[0]);
    uartDriverWriteByte(GET_STRING_EXTENDS[1]);
    char c = text[0];
    uint8_t i = 0;
    while (c != 0) {
        uartDriverWriteByte(c);
        i++;
        c = text[i];
    }   
    return 0;
}

void GLK_InitializeTextWindow(uint8_t id, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t font, uint8_t char_space, uint8_t line_space, uint8_t scroll) {
    uint8_t data[12];
    data[0] = INIT_TEXT_WINDOW[0];
    data[1] = INIT_TEXT_WINDOW[1];
    data[2] = id;
    data[3] = x1;
    data[4] = y1;
    data[5] = x2;
    data[6] = y2;
    data[7] = font & 0x00FF;
    data[8] = (font >> 8) & 0x00FF;
    data[9] = char_space;
    data[10] = line_space;
    data[11] = scroll;
    write(data, 12);
}

void GLK_SetTextWindow(uint8_t id) {
    uint8_t data[3];
    data[0] = SET_TEXT_WINDOW[0];
    data[1] = SET_TEXT_WINDOW[1];
    data[2] = id;
    write(data, 3);
}


void GLK_ClearTextWindow(uint8_t id) {
    uint8_t data[3];
    data[0] = CLEAR_TEXT_WINDOW[0];
    data[1] = CLEAR_TEXT_WINDOW[1];
    data[2] = id;
    write(data, 3);
}


void GLK_InitializeLabel(uint8_t id, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t vert, uint8_t hor, uint16_t font, uint8_t background, uint8_t char_space) {
    uint8_t data[13];
    data[0] = INIT_LABEL[0];
    data[1] = INIT_LABEL[1];
    data[2] = id;
    data[3] = x1;
    data[4] = y1;
    data[5] = x2;
    data[6] = y2;
    data[7] = vert;
    data[8] = hor;
    data[9] = font & 0x00FF;
    data[10] = (font >> 8) & 0x00FF;
    data[11] = background;
    data[12] = char_space;
    write(data, 13);
}

void GLK_InitializeScrollingLabel(uint8_t id, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t vert, uint8_t dir, uint16_t font, uint8_t background, uint8_t char_space, uint16_t delay) {
    uint8_t data[15];
    data[0] = INIT_SCROLLING_LABEL[0];
    data[1] = INIT_SCROLLING_LABEL[1];
    data[2] = id;
    data[3] = x1;
    data[4] = y1;
    data[5] = x2;
    data[6] = y2;
    data[7] = vert;
    data[8] = dir;
    data[9] = font & 0x00FF;
    data[10] = (font >> 8) & 0x00FF;
    data[11] = background;
    data[12] = char_space;
    data[13] = delay & 0x00FF;
    data[14] = (delay >> 8) & 0x00FF;
    write(data, 15);
}

void GLK_UpdateLabel(uint8_t id, const char * data) {

    uartDriverWriteByte(UPDATE_LABEL[0]);
    uartDriverWriteByte(UPDATE_LABEL[1]);
    uartDriverWriteByte(id);
    
    char c = 1;
    while (c != 0) {
        c = *(data++);
        uartDriverWriteByte(c);
    }
}

void GLK_AutoScrollOn(void) {
    write(AUTO_SCROLL_ON, 2);
}


void GLK_AutoScrollOff(void) {
    write(AUTO_SCROLL_OFF, 2);
}



/********************************************************************************
 *              DRAWING
 *******************************************************************************/
void GLK_SetDrawingColor(uint8_t color) {
    uint8_t data[3];
    data[0] = SET_DRAWING_COLOR[0];
    data[1] = SET_DRAWING_COLOR[1];
    data[2] = color;
    write(data, 3);
}


void GLK_DrawPixel(uint8_t x, uint8_t y) {
    uint8_t data[4];
    data[0] = DRAW_PIXEL[0];
    data[1] = DRAW_PIXEL[1];
    data[2] = x;
    data[3] = y;
    write(data, 4);
}


void GLK_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    uint8_t data[6];
    data[0] = DRAW_LINE[0];
    data[1] = DRAW_LINE[1];
    data[2] = x1;
    data[3] = y1;
    data[4] = x2;
    data[5] = y2;
    write(data, 6);
}


void GLK_ContinueLine(uint8_t x, uint8_t y) {
    uint8_t data[4];
    data[0] = CONTINUE_LINE[0];
    data[1] = CONTINUE_LINE[1];
    data[2] = x;
    data[3] = y;
    write(data, 4);
}

void GLK_DrawRectangle(uint8_t color, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    uint8_t data[7];
    data[0] = DRAW_RECTANGLE[0];
    data[1] = DRAW_RECTANGLE[1];
    data[2] = color;
    data[3] = x1;
    data[4] = y1;
    data[5] = x2;
    data[6] = y2;
    write(data, 7);
}

void GLK_DrawFilledRectangle(uint8_t color, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    uint8_t data[7];
    data[0] = DRAW_FILLED_RECT[0];
    data[1] = DRAW_FILLED_RECT[1];
    data[2] = color;
    data[3] = x1;
    data[4] = y1;
    data[5] = x2;
    data[6] = y2;
    write(data, 7);
}

void GLK_DrawRoundedRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t radius) {
    uint8_t data[7];
    data[0] = DRAW_ROUNDED_RECT[0];
    data[1] = DRAW_ROUNDED_RECT[1];
    data[2] = x1;
    data[3] = y1;
    data[4] = x2;
    data[5] = y2;
    data[6] = radius;
    write(data, 7);
}


/********************************************************************************
 *              GPIO
 *******************************************************************************/
void GLK_GeneralPurposeOutputOn(uint8_t number) {
    uint8_t data[3];
    data[0] = GPO_ON[0];
    data[1] = GPO_ON[1];
    data[2] = number;
    write(data, 3);
}

void GLK_GeneralPurposeOutputOff(uint8_t number) {
    uint8_t data[3];
    data[0] = GPO_OFF[0];
    data[1] = GPO_OFF[1];
    data[2] = number;
    write(data, 3);
}

void GLK_SetStartUpGPOState(uint8_t number, uint8_t state) {
    uint8_t data[4];
    data[0] = SET_START_UP_STATE[0];
    data[1] = SET_START_UP_STATE[1];
    data[2] = number;
    data[3] = state;
    write(data, 4);
}

/********************************************************************************
 *              LED Indicators
 *******************************************************************************/
void GLK_SetLEDIndicators(GLKLed_t number, LEDState_t color) {
    uint8_t data[4];
    data[0] = SET_LED_INDICATORS[0];
    data[1] = SET_LED_INDICATORS[1];
    data[2] = number;
    data[3] = color;
    write(data, 4);
}


/********************************************************************************
 *              MISC
 *******************************************************************************/
void GLK_ReadVersionNumber(uint8_t * response) {
    _U1RXIE = 0; // Disable interrupts
    write(READ_VERSION_NUMBER, 2);
    
    _U1RXIF = 0; // Clear flag
    
    while(_U1RXIF == 0);
    *response = U1RXREG;
    _U1RXIF = 0;
    _U1RXIE = 1; // Enable interrupts
    
}

void GLK_ReadModuleType(uint8_t * response) {
    _U1RXIE = 0; // Disable interrupts
    write(READ_MODULE_NUMBER, 2);
    
    while(_U1RXIF == 0);
    *response = U1RXREG;
    _U1RXIF = 0;
    _U1RXIE = 1; // Enable interrupts
}




























