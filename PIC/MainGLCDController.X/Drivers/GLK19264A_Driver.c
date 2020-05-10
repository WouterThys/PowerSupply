#include "GLK19264A_Driver.h"


/********************************************************************************
 *              COMMANDS 
 *******************************************************************************/

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


/********************************************************************************
 *              PROTOTYPES 
 *******************************************************************************/
static void uartDataRead(uint8_t data);
static void write(const uint8_t * command, const uint8_t length);


/********************************************************************************
 *              INTERNAL FUNCTIONS 
 *******************************************************************************/

static void uartDataRead(uint8_t data) {

}

static void write(const uint8_t * command, const uint8_t length) {
    uint8_t i = 0;
    for (i = 0; i < length; i++) {
        UART_PutChar(command[i]);
    }
}

/********************************************************************************
 *              DRIVER FUNCTIONS
 *******************************************************************************/

void GLK_Init() {


    // Initialize UART
    UART_Init(UART_BAUD, UART_INVERT, uartDataRead);   
}

void GLK_Write(const char * text) {

    char c = text[0];
    uint8_t i = 0;

    while (c != 0) {
        UART_PutChar(c);
        i++;
        c = text[i];
    }
}

void GLK_ClearScreen() {
    write(&CLEAR_SCREEN[0], 2);
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

    UART_PutChar(GET_STRING_EXTENDS[0]);
    UART_PutChar(GET_STRING_EXTENDS[1]);
    char c = text[0];
    uint8_t i = 0;
    while (c != 0) {
        UART_PutChar(c);
        i++;
        c = text[i];
    }   
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
    uint8_t data[14];
    data[0] = INIT_LABEL[0];
    data[1] = INIT_LABEL[1];
    data[2] = id;
    data[3] = x1;
    data[4] = y1;
    data[5] = x2;
    data[6] = y2;
    data[7] = vert;
    data[9] = hor;
    data[10] = font & 0x00FF;
    data[11] = (font >> 8) & 0x00FF;
    data[12] = background;
    data[13] = char_space;
    write(data, 14);
}

void GLK_InitializeScrollingLabel(uint8_t id, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t vert, uint8_t dir, uint16_t font, uint8_t background, uint8_t char_space, uint16_t delay) {
    uint8_t data[16];
    data[0] = INIT_SCROLLING_LABEL[0];
    data[1] = INIT_SCROLLING_LABEL[1];
    data[2] = id;
    data[3] = x1;
    data[4] = y1;
    data[5] = x2;
    data[6] = y2;
    data[7] = vert;
    data[9] = dir;
    data[10] = font & 0x00FF;
    data[11] = (font >> 8) & 0x00FF;
    data[12] = background;
    data[13] = char_space;
    data[14] = delay & 0x00FF;
    data[15] = (delay >> 8) & 0x00FF;
    write(data, 16);
}

void GLK_UpdateLabel(uint8_t id, const char * data) {

    UART_PutChar(UPDATE_LABEL[0]);
    UART_PutChar(UPDATE_LABEL[1]);
    char c = data[0];
    uint8_t i = 0;
    while (c != 0) {
        UART_PutChar(c);
        i++;
        c = data[i];
    } 
}

void GLK_AutoScrollOn(void) {
    write(AUTO_SCROLL_ON, 2);
}


void GLK_AutoScrollOff(void) {
    write(AUTO_SCROLL_OFF, 2);
}










































