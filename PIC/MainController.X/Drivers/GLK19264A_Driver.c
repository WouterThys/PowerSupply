#include "GLK19264A_Driver.h"

#include "Bitmaps.h"

/********************************************************************************
 *              COMMANDS 
 *******************************************************************************/
// COMMUNICATION
static const uint8_t CHANGE_BAUD_RATE[2] = {0xFE, 0x39};

// TEXT
static const uint8_t CLEAR_SCREEN[2] = {0xFE, 0x58};
static const uint8_t GO_HOME[2] = {0xFE, 0x48};
static const uint8_t SET_CURSOR_POS[2] = {0xFE, 0x47};
static const uint8_t SET_CURSOR_COORD[2] = {0xFE, 0x79};
static const uint8_t GET_STRING_EXTENDS[2] = {0xFE, 0x29};
static const uint8_t INIT_TEXT_WINDOW[2] = {0xFE, 0x2B};
static const uint8_t SET_TEXT_WINDOW[2] = {0xFE, 0x2A};
static const uint8_t CLEAR_TEXT_WINDOW[2] = {0xFE, 0x2C};
static const uint8_t INIT_LABEL[2] = {0xFE, 0x2D};
static const uint8_t INIT_SCROLLING_LABEL[2] = {0xFE, 0x2F};
static const uint8_t UPDATE_LABEL[2] = {0xFE, 0x2E};
static const uint8_t AUTO_SCROLL_ON[2] = {0xFE, 0x51};
static const uint8_t AUTO_SCROLL_OFF[2] = {0xFE, 0x52};

// DAWING
static const uint8_t SET_DRAWING_COLOR[2] = {0xFE, 0x63};
static const uint8_t DRAW_PIXEL[2] = {0xFE, 0x70};
static const uint8_t DRAW_LINE[2] = {0xFE, 0x6C};
static const uint8_t CONTINUE_LINE[2] = {0xFE, 0x65};
static const uint8_t DRAW_RECTANGLE[2] = {0xFE, 0x72};
static const uint8_t DRAW_FILLED_RECT[2] = {0xFE, 0x78};
static const uint8_t DRAW_ROUNDED_RECT[2] = {0xFE, 0x80};
static const uint8_t DRAW_CIRCLE[2] = {0xFE, 0x7B};
static const uint8_t DRAW_FILLED_CIRCLE[2] = {0xFE, 0x7C};

// BITMAPS
static const uint8_t UPLOAD_BITMAP_FILE[2] = {0xFE, 0x5E};
static const uint8_t UPLOAD_BITMAP_MASK[3] = {0xFE, 0x5C, 0x05};
static const uint8_t DRAW_BITMAP[2] = {0xFE, 0x62};

// GPIO
static const uint8_t GPO_ON[2] = {0xFE, 0x57};
static const uint8_t GPO_OFF[2] = {0xFE, 0x56};
static const uint8_t SET_START_UP_STATE[2] = {0xFE, 0xC3};

// LED Indicators
static const uint8_t SET_LED_INDICATORS[2] = {0xFE, 0x5A};

// KEYPAD
static const uint8_t AUTO_TRANSMIT_KEY_ON[2] = {0xFE, 0x41};
static const uint8_t AUTO_TRANSMIT_KEY_OFF[2] = {0xFE, 0x4F};
static const uint8_t POLL_KEY_PRESS[2] = {0xFE, 0x26};
static const uint8_t CLEAR_KEY_BUFFER[2] = {0xFE, 0x45};
static const uint8_t SET_DEBOUNCE_TIME[2] = {0xFE, 0x55};
static const uint8_t SET_AUTO_REPEAT_MODE[2] = {0xFE, 0x7E};
static const uint8_t AUTO_REPEAT_MODE_OFF[2] = {0xFE, 0x60};
static const uint8_t ASSIGN_KEYPAD_CODES[2] = {0xFE, 0xD5};
static const uint8_t KEYPAD_BACKLIGHT_OFF[2] = {0xFE, 0x9B};
static const uint8_t SET_KEYPAD_BRIGHTNESS[2] = {0xFE, 0x9C};
static const uint8_t SET_AUTO_BACKLIGHT[2] = {0xFE, 0x9D};
static const uint8_t SET_TYPEMATIC_DELAY[2] = {0xFE, 0x9F};
static const uint8_t SET_TYPEMATIC_INTERVAL[2] = {0xFE, 0x9E};

// MISC
static const uint8_t READ_VERSION_NUMBER[2] = {0xFE, 0x36};
static const uint8_t READ_MODULE_NUMBER[2] = {0xFE, 0x37};

/********************************************************************************
 *              PROTOTYPES 
 *******************************************************************************/
static void uartDataRead(uint8_t data);
static void write(const uint8_t * command, const uint8_t length);
static void read(uint8_t * data);
static buttonCallback btnCallback;

/********************************************************************************
 *              INTERNAL FUNCTIONS 
 *******************************************************************************/

static void uartDataRead(uint8_t data) {
    btnCallback((data & 0x7F));
}

static void write(const uint8_t * command, const uint8_t length) {
    uint8_t i = 0;
    for (i = 0; i < length; i++) {
        uart2DriverWrite(command[i]);
    }
}

static void read(uint8_t * data) {
    while (_U2RXIF == 0);
    *data = U2RXREG;
    _U2RXIF = 0;
}

/********************************************************************************
 *              DRIVER FUNCTIONS
 *******************************************************************************/

void GLK_Init(buttonCallback callback) {
    // Callback
    btnCallback = callback;

    // Initialize UART
    uart2DriverInit(UART2_BAUD, uartDataRead);
    uart2DriverEnable(true);

    // Write bitmaps
    if (UPLOAD_BITMAPS) {
        GLK_UploadBitmapFile(CHECK_FALSE_ID, CHECK_FALSE_SIZE, &(_check_false[0]));
        GLK_UploadBitmapFile(CHECK_TRUE_ID, CHECK_TRUE_SIZE, &(_check_true[0]));
    }
}

void GLK_WriteText(uint8_t x, uint8_t y, const char * text) {

    // Go to positions
    GLK_SetCursorCoordinate(x, y);

    char c = text[0];
    uint8_t i = 0;

    while (c != 0) {
        uart2DriverWrite(c);
        i++;
        c = text[i];
    }
}

/********************************************************************************
 *              COMMUNICATION
 *******************************************************************************/

void GLK_ChangeBaudRate(GLKBaud_t speed) {
    uint8_t data[3];
    data[0] = CHANGE_BAUD_RATE[0];
    data[1] = CHANGE_BAUD_RATE[1];
    data[2] = (uint8_t) speed;
    write(data, 3);
}

/********************************************************************************
 *              TEXT
 *******************************************************************************/

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

    uart2DriverWrite(GET_STRING_EXTENDS[0]);
    uart2DriverWrite(GET_STRING_EXTENDS[1]);
    char c = text[0];
    uint8_t i = 0;
    while (c != 0) {
        uart2DriverWrite(c);
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

    uart2DriverWrite(UPDATE_LABEL[0]);
    uart2DriverWrite(UPDATE_LABEL[1]);
    uart2DriverWrite(id);

    char c = 1;
    while (c != 0) {
        c = *(data++);
        uart2DriverWrite(c);
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

void GLK_DrawCircle(uint8_t x, uint8_t y, uint8_t radius) {
    uint8_t data[5];
    data[0] = DRAW_CIRCLE[0];
    data[1] = DRAW_CIRCLE[1];
    data[2] = x;
    data[3] = y;
    data[4] = radius;
    write(data, 5);
}

void GLK_DrawFilledCircle(uint8_t x, uint8_t y, uint8_t radius) {
    uint8_t data[5];
    data[0] = DRAW_FILLED_CIRCLE[0];
    data[1] = DRAW_FILLED_CIRCLE[1];
    data[2] = x;
    data[3] = y;
    data[4] = radius;
    write(data, 5);
}

/********************************************************************************
 *              BITMAPS
 *******************************************************************************/

void GLK_UploadBitmapFile(uint16_t id, uint32_t size, const uint8_t * data) {

    // Disable interrupts
    _U2RXIE = 0;
    uint8_t response = 0;

    // Command
    uart2DriverWrite(UPLOAD_BITMAP_FILE[0]);
    uart2DriverWrite(UPLOAD_BITMAP_FILE[1]);

    // Id
    uart2DriverWrite(id & 0x00FF);
    uart2DriverWrite((id >> 8) & 0x00FF);

    // Size
    uart2DriverWrite(size & 0x00FF);
    uart2DriverWrite((size >> 8) & 0x00FF);
    uart2DriverWrite((size >> 16) & 0x00FF);
    uart2DriverWrite((size >> 24) & 0x00FF);

    // Wait for ACK
    read(&response);
    if (response != 1) {
        // Failed
        return;
    }

    // Data
    uint8_t i;
    uint8_t d;
    uint8_t ack = 0;
    for (i = 0; i < size; i++) {
        do {
            d = data[i];
            uart2DriverWrite(d);
            read(&response);
            if (response == d) {
                ack = 1;
            } else {
                ack = 8;
            }
            uart2DriverWrite(ack);
        } while (ack != 1);
    }

    // Enable interrupts again
    _U2RXIE = 1;
}

void GLK_UploadBitmapMask(uint16_t id, uint32_t size, const uint8_t * data) {

    // Command
    uart2DriverWrite(UPLOAD_BITMAP_MASK[0]);
    uart2DriverWrite(UPLOAD_BITMAP_MASK[1]);
    uart2DriverWrite(UPLOAD_BITMAP_MASK[2]);

    // Id
    uart2DriverWrite(id & 0x00FF);
    uart2DriverWrite((id >> 8) & 0x00FF);

    // Size
    uart2DriverWrite(size & 0x000000FF);
    uart2DriverWrite((size >> 8) & 0x000000FF);
    uart2DriverWrite((size >> 16) & 0x000000FF);
    uart2DriverWrite((size >> 24) & 0x000000FF);

    // Data
    uint8_t i;
    for (i = 0; i < size; i++) {
        uart2DriverWrite(data[i]);
    }
}

void GLK_DrawBitmapFromMemory(uint16_t id, uint8_t x, uint8_t y) {
    uint8_t data[6];
    data[0] = DRAW_BITMAP[0];
    data[1] = DRAW_BITMAP[1];
    data[2] = id & 0x00FF;
    data[3] = (id >> 8) & 0x00FF;
    data[4] = x;
    data[5] = y;
    write(data, 6);
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
 *              KEYPAD
 *******************************************************************************/

void GLK_AutoTransmitKeyPressedOn() {
    write(AUTO_TRANSMIT_KEY_ON, 2);
}

void GLK_AutoTransmitKeyPressedOff() {
    write(AUTO_TRANSMIT_KEY_OFF, 2);
}

void GLK_PollKeyPress(uint8_t * response) {
    write(POLL_KEY_PRESS, 2);
    *response = 0;
}

void GLK_ClearKeyBuffer() {
    write(CLEAR_KEY_BUFFER, 2);
}

void GLK_SetDebounceTime(uint8_t time) {
    uint8_t data[43];
    data[0] = SET_DEBOUNCE_TIME[0];
    data[1] = SET_DEBOUNCE_TIME[1];
    data[2] = time;
    write(data, 3);
}

void GLK_SetAutoRepeatMode(uint8_t mode) {
    uint8_t data[43];
    data[0] = SET_AUTO_REPEAT_MODE[0];
    data[1] = SET_AUTO_REPEAT_MODE[1];
    data[2] = mode;
    write(data, 3);
}

void GLK_AutoRepeatModeOff() {
    write(AUTO_REPEAT_MODE_OFF, 2);
}

void GLK_AssingKeypadCodes(uint8_t key_down[9], uint8_t key_up[9]) {

}

void GLK_KeypadBacklightOff() {
    write(KEYPAD_BACKLIGHT_OFF, 2);
}

void GLK_SetKeypadBrightness(uint8_t brightness) {
    uint8_t data[43];
    data[0] = SET_KEYPAD_BRIGHTNESS[0];
    data[1] = SET_KEYPAD_BRIGHTNESS[1];
    data[2] = brightness;
    write(data, 3);
}

void GLK_SetAutoBacklight(uint8_t setting) {
    uint8_t data[43];
    data[0] = SET_AUTO_BACKLIGHT[0];
    data[1] = SET_AUTO_BACKLIGHT[1];
    data[2] = setting;
    write(data, 3);
}

void GLK_SetTypematicDelay(uint8_t delay) {
    uint8_t data[43];
    data[0] = SET_TYPEMATIC_DELAY[0];
    data[1] = SET_TYPEMATIC_DELAY[1];
    data[2] = delay;
    write(data, 3);
}

void GLK_SetTypematicInterval(uint8_t interval) {
    uint8_t data[43];
    data[0] = SET_TYPEMATIC_INTERVAL[0];
    data[1] = SET_TYPEMATIC_INTERVAL[1];
    data[2] = interval;
    write(data, 3);
}

/********************************************************************************
 *              MISC
 *******************************************************************************/
void GLK_ReadVersionNumber(uint8_t * response) {
    _U2RXIE = 0; // Disable interrupts
    write(READ_VERSION_NUMBER, 2);

    _U2RXIF = 0; // Clear flag

    while (_U2RXIF == 0);
    *response = U2RXREG;
    _U2RXIF = 0;
    _U2RXIE = 1; // Enable interrupts

}

void GLK_ReadModuleType(uint8_t * response) {
    _U2RXIE = 0; // Disable interrupts
    write(READ_MODULE_NUMBER, 2);

    while (_U2RXIF == 0);
    *response = U2RXREG;
    _U2RXIF = 0;
    _U2RXIE = 1; // Enable interrupts
}




























