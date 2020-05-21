#ifndef GLK19264A_DRIVER_H
#define GLK19264A_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include "../Settings.h"
#include "UART2_Driver.h"

#define BTN_TOP     'B'
#define BTN_RIGHT   'C'
#define BTN_BOTTOM  'H'
#define BTN_LEFT    'D'
#define BTN_CENTER  'E'
#define BTN_1       'A'
#define BTN_2       'G'

/********************************************************************************
 *              ENUMS
 *******************************************************************************/

typedef enum GLKBaud {
    B9600   = 207,
    B14400  = 138,
    B19200  = 103,
    B28800  = 68,
    B38400  = 51,
    B57600  = 34,
    B76800  = 25,
    B115200 = 16
} GLKBaud_t;

typedef enum VerticalJustification {
    Top = 0,
    Middle = 1,
    Bottom = 2
} VerticalJustification_t;

typedef enum HorizontalJustification {
    Left = 0,
    Center = 1,
    Right = 2
} HorizontalJustification_t; 

typedef enum BarGraphType {
    VerticalBottom  = 0,
    HorizontalLeft  = 1,
    VerticalTop     = 2,
    HorizontalRight = 3
} BarGraphType_t;     

typedef enum GLKLed {
    LTop    = 0,
    LMiddle = 1,
    LBottom = 2
} GLKLed_t;

typedef enum LEDState {
    Off     = 0,
    Green   = 1,
    Red     = 2,
    Yellow  = 3
} LEDState_t;

/********************************************************************************
 *              FUCTIONS
 *******************************************************************************/
typedef void (*buttonCallback)(uint8_t button);

/********************************************************************************
 *              MY FUNCTIONS
 *******************************************************************************/

void GLK_Init(buttonCallback callback);

void GLK_WriteText(uint8_t x, uint8_t y, const char * text);

/********************************************************************************
 *              COMMUNICATION
 *******************************************************************************/

/**
 * Change Baud Rate: Immediately changes the baud rate. Not available in I2C.
 * Baud rate can be temporarily forced to 19200 by a manual override.
 * @param speed: Valid settings shown below.
 */
void GLK_ChangeBaudRate(GLKBaud_t speed);

/********************************************************************************
 *              TEXT
 *******************************************************************************/
/**
 * Clear screen: Clear the contents of the screen.
 */
void GLK_ClearScreen(void);

/**
 * Go Home: Returns the cursor to the top left of the screen.
 */
void GLK_GoHome(void);

/*
 * Set Cursor Position: Sets the cursor to a specific cursor position where 
 * the next transmitted character is printed.
 * @param c: column, value between 1 and number of character columns.
 * @param r: row, Value between 1 and number of character rows.
 */
void GLK_SetCursorPosition(uint8_t c, uint8_t r);

/*
 * Set Cursor Coordinate: Sets the cursor to an exact pixel position where
 * the next transmitted character is printed.
 * @param x: Value between 1 and screen width, represents leftmost character position.
 * @param y: Value between 1 and screen height, represents topmost character position.
 */
void GLK_SetCursorCoordinate(uint8_t x, uint8_t y);

/*
 * Get String Extends: Read the size of the rectangle that the specified string would
 * occupy if it was rendered with the current font.
 * @param text: String on which to preform extents calculation. A single line of text is assumed.
 * @return Width and height of the string in pixels. A width greater than the screen will return 0.
 */
uint8_t * GLK_GetStringExtends(const char * text);

/*
 * Initialize Text Window: Designates a portion of the screen to which text can be confined. 
 * Font commands affect only the current window, default (entire screen) is window 0.
 * @param id: Unique text window identification number, value between 0 and 15.
 * @param x1: Leftmost coordinate. 
 * @param y1: Topmost coordinate.
 * @param x2: Rightmost coordinate.
 * @param y2: Bottommost coordinate.
 * @param font: Unique font ID to use for this window, value between 0 and 1023.
 * @param char_space: Spacing between characters to use for this window.
 * @param line_space: Spacing between lines to use for this window.
 * @param scroll: Number of pixel rows to write to before scrolling text.
 */
void GLK_InitializeTextWindow(uint8_t id, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t font, uint8_t char_space, uint8_t line_space, uint8_t scroll);

/*
 * Set Text Window: Sets the text window to which subsequent text and commands will apply. 
 * Default (entire screen) is window 0.
 * @param id: Unique text window to use.
 */
void GLK_SetTextWindow(uint8_t id);

/*
 * Clear Text Window: Clear the contents of a specific text window, similar to the clear screen command.
 * @param id: Unique text window to clear.
 */
void GLK_ClearTextWindow(uint8_t id);

/*
 * Initialize Label: Designates a portion of the screen that can be easily updated with one line of text,
 * often used to display variables.
 * @param id: Unique label identification number, value between 0 and 15.
 * @param x1: Leftmost coordinate.
 * @param y1: Topmost coordinate.
 * @param x2: Rightmost coordinate.
 * @param y2: Bottommost coordinate.
 * @param vert: Vertical justification of the text; o for top, 1 for middle, or 2 for bottom.
 * @param hor: Horizontal justification of the label text; 0 for left, 1 for center, or 2 for right.
 * @param font: Unique font ID to use for this label, value between 0 and 1023.
 * @param background: State of the pixels in the label region that is not occupied by text; 0 for off or 1 for on.
 * @param char_space: Spacing between characters to use for this label.
 */
void GLK_InitializeLabel(uint8_t id, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t vert, uint8_t hor, uint16_t font, uint8_t background, uint8_t char_space);

/*
 * Initialize Scrolling Label: Designates a portion of the screen that can be easily updated with one line of text,
 * often used to display variables.
 * @param id: Unique label identification number, value between 0 and 15.
 * @param x1: Leftmost coordinate.
 * @param y1: Topmost coordinate.
 * @param x2: Rightmost coordinate.
 * @param y2: Bottommost coordinate.
 * @param vert: Vertical justification of the label text; 0 for top, 1 for middle, or 2 for bottom.
 * @param dir: Direction of the scrolling behavior; 0 for left, 1 for right, or 2 for bounce.
 * @param font: Unique font ID to use for this label, value between 0 and 1023.
 * @param background: State of the pixels in the label region that is not occupied by text; 0 for off or 1 for on.
 * @param char_space: Spacing between characters to use for this label.
 * @param delay: Time in milliseconds to elapse between characters printed.
 */
void GLK_InitializeScrollingLabel(uint8_t id, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t vert, uint8_t dir, uint16_t font, uint8_t background, uint8_t char_space, uint16_t delay);

/*
 * Update Label: Update a previously created label or scrolling label with new text.
 * Send a null character (empty string) to clear.
 * @param id: Unique label to update, value between 0 and 15.
 * @param data: Information to display in label, must be terminated with a null (value of zero) byte.
 */
void GLK_UpdateLabel(uint8_t id, const char * data);

/*
 * Auto Scroll On: The entire contents of the screen are shifted up one line when the end of the screen reached.
 * Display default is on.
 */
void GLK_AutoScrollOn(void);

/*
 * Auto Scroll Off: New text is written over the top line when the end of the screen is reached.
 * Display default Auto Scroll on.
 */
void GLK_AutoScrollOff(void);


/********************************************************************************
 *              DRAWING
 *******************************************************************************/

/*
 * Set Drawing Color: Set the color to be used for all future drawing commands
 * that do not implicitly specify color.
 * @param color: 0 for background or any other value for text color.
 */
void GLK_SetDrawingColor(uint8_t color);

/*
 * Draw Pixel: Draw a single pixel at the specified coordinate using the current
 * drawing color.
 * @param x: Horizontal position of pixel to be drawn.
 * @param y: Vertical position of pixel to be drawn.
 */
void GLK_DrawPixel(uint8_t x, uint8_t y);

/*
 * Draw Line: Draw a line connecting two termini. Lines may be rendered differently
 * when drawn right to left versus left to right.
 * @param x1: Horizontal coordinate of first terminus.
 * @param y1: Vertical coordinate of first terminus.
 * @param x2: Horizontal coordinate of second terminus.
 * @param y2: Vertical coordinate of second terminus.
 */
void GLK_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

/*
 * Continue Line: Draw a line from the last point drawn to the coordinate specified
 * using the current drawing color.
 * @param x: Left coordinate of terminus.
 * @param y: Top coordinate of terminus.
 */
void GLK_ContinueLine(uint8_t x, uint8_t y);

/*
 * Draw Rectangle: Draw a rectangular frame one pixel wide using the color specified.
 * Current drawing color is ignored.
 * @param color: 0 for background or any other value for text color.
 * @param x1: Lefmost coordinate.
 * @param y1: Topmost coordinate.
 * @param x2: Rightmost coordinate.
 * @param y2: Bottommost coordinate.
 */
void GLK_DrawRectangle(uint8_t color, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

/*
 * Draw filled Rectangle: Draw a filled rectangle using the color specified.
 * Current drawing color is ignored.
 * @param color: 0 for background or any other value for text color.
 * @param x1: Lefmost coordinate.
 * @param y1: Topmost coordinate.
 * @param x2: Rightmost coordinate.
 * @param y2: Bottommost coordinate.
 */
void GLK_DrawFilledRectangle(uint8_t color, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

/*
 * Draw Rounded Rectangle: Draw a rounded rectangular frame one pixel wide
 * using the current drawing color.
 * @param x1: Leftmost coordinate of the rectangle.
 * @param y1: Topmost coordinate of the rectangle.
 * @param x2: Rightmost coordinate of the rectangle.
 * @param y2: Bottommost coordinate of the rectangle.
 * @param radius: Radius of curvature of the rectangle corners.
 */
void GLK_DrawRoundedRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t radius);

/*
 * Draw Filled Rounded Rectangle: Draw a filled rounded rectangular frame one pixel wide
 * using the current drawing color.
 * @param x1: Leftmost coordinate of the rectangle.
 * @param y1: Topmost coordinate of the rectangle.
 * @param x2: Rightmost coordinate of the rectangle.
 * @param y2: Bottommost coordinate of the rectangle.
 * @param radius: Radius of curvature of the rectangle corners.
 */

void GLK_DrawFilledRoundedRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t radius);

/*
 * Draw Circle: Draw a circular frame one pixel wide using the current drawing color.
 * @param x: Horizontal coordinate of the circle center.
 * @param y: Vertical coordinate of the circle center.
 * @param radius: Distance between the circle perimeter and center.
 */
void GLK_DrawCircle(uint8_t x, uint8_t y, uint8_t radius);

/*
 * Draw Filled Circle: Draw a filled circular frame one pixel wide using the current drawing color.
 * @param x: Horizontal coordinate of the circle center.
 * @param y: Vertical coordinate of the circle center.
 * @param radius: Distance between the circle perimeter and center.
 */
void GLK_DrawFilledCircle(uint8_t x, uint8_t y, uint8_t radius);

/*
 * Draw Ellipse: Draw an elliptical frame one pixel wide using the current drawing color.
 * @param x: Horizontal coordinate of the circle center, zero indexed from left.
 * @param y: Vertical coordinate of the circle center, zero indexed from top.
 * @param x_radius: Distance between the furthest horizontal point on the ellipse perimneter and center.
 * @param y_radius: Distance between the furthest vertical point on the ellipse perimneter and center.
 */
void GLK_DrawEllipse(uint8_t x, uint8_t y, uint8_t x_radius, uint8_t y_radius);

/*
 * Draw Filled Ellipse: Draw an filled elliptical frame one pixel wide using the current drawing color.
 * @param x: Horizontal coordinate of the circle center, zero indexed from left.
 * @param y: Vertical coordinate of the circle center, zero indexed from top.
 * @param x_radius: Distance between the furthest horizontal point on the ellipse perimneter and center.
 * @param y_radius: Distance between the furthest vertical point on the ellipse perimneter and center.
 */

void GLK_DrawFilledEllipse(uint8_t x, uint8_t y, uint8_t x_radius, uint8_t y_radius);

/*
 * Scroll Screen: Define and scroll the contents of a portion of the screen.
 * @param x1: Leftmost coordinate of the scroll window, zero indexed from left.
 * @param y1: Topmost coordinate of the scroll window,zero indexed from top.
 * @param x2: Rightmost coordinate of the scroll window, zero indexed from left.
 * @param y2: Bottommost coordinate of the scroll window, zero indexed from top.
 * @param move_x: Number of pixels to scroll horizontally.
 * @param move_y: Number of pixels to scroll vertically.
 */
void GLK_ScrollScreen(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, int16_t move_x, int16_t move_y);

/* 
 * Initialize Bar Graph: Initialize a bar graph in memory for later implementation. Graphs can be 
 * located anywhere on the screen, but overlapping may cause distortion. Graph should be filled using 
 * the Draw Bar command.
 * @param id: Unique bar identification number, between 0 and 255.
 * @param type: Graph style, see Bar Graph Type.
 * @param x1: Leftmost coordinate. 
 * @param y1: Topmost coordinate.
 * @param x2: Rightmost coordinate.
 * @param y2: Bottommost coordinate.
 */
void GLK_InitializeBarGraph(uint8_t id, BarGraphType_t type, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

void GLK_Initialize9SliceBarGraph(uint8_t id, BarGraphType_t type, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t fore_slice, uint16_t back_slice);

void GLK_DrawBarGraph(uint8_t id, uint8_t value);

void GLK_InitializeStripChart(uint8_t id, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, int16_t min, int16_t max, uint8_t step, uint16_t file_id);

void GLK_UpdateStripChart(uint8_t id, int16_t value);

/********************************************************************************
 *              BITMAPS
 *******************************************************************************/

/**
 * Upload a Bitmap File: Upload a bitmap to a graphic display. To create a bitmap
 * see the Bitmap File Creation section. for upload protocol see the 
 * File Transfer Protocol for XModem Transfer Protocol entries. Start screen is ID 1.
 * @param id: Unique bitmap identification number, value between 0 and 1023.
 * @param size: Size of the entire bitmap file
 * @param data: Bitmap file data, see the Bitmap File Creation example.
 */
void GLK_UploadBitmapFile(uint16_t id, uint32_t size, const uint8_t * data);

/**
 * Upload a Bitmap Mask: Upload a bitmap mask that can clear areas of the screen
 * before a bitmap is drawn. Programmatically, (bitmap&mask)|(screen&~mask) is 
 * shown when a bitmap is drawn. To create a mask see the Bitmap Masking section,
 * for upload protocol see the File Transfer Protocol or XModem Transfer Protocol entries.
 * @param id
 * @param size
 * @param data
 */
void GLK_UploadBitmapMask(uint16_t id, uint32_t size, const uint8_t * data);

/**
 * Draw a Bitmap From Memory: Draw a previously uploaded bitmap from memory. 
 * Top left corner must be specified for drawing.
 * @param id: Unique bitmap identification number, value between 0 and 1023.
 * @param x: Leftmost coordinate of bitmap.
 * @param y: Rightmost coordinate of bitmap.
 */
void GLK_DrawBitmapFromMemory(uint16_t id, uint8_t x, uint8_t y);

/********************************************************************************
 *              GPIO
 *******************************************************************************/

/*
 * General Purpose Output On: Turns the specified GPO on, sourcing current from 
 * an output of five volts.
 * @param number: GPO to be turned on.
 */
void GLK_GeneralPurposeOutputOn(uint8_t number);

/*
 * General Purpose Output Off: Turns the specified GPO off, singking current to 
 * an output of zero volts.
 * @param number: GPO to be turned off.
 */
void GLK_GeneralPurposeOutputOff(uint8_t number);

/*
 * Set Start Up GPO State: Sets and saves the start up state of the specified GPO
 * in non volatile memory. Changes will be seen on start up.
 * @param number: GPO to be controlled.
 * @param state: 1 for on or 0 for off.
 */
void GLK_SetStartUpGPOState(uint8_t number, uint8_t state);



/********************************************************************************
 *              LED Indicators
 *******************************************************************************/

/*
 * Set LED Indicators: Immediately sets the state of the specified LED indicator
 * to a specific color. Temporary unless remember is on. LED indicators are 
 * numbered from 0 to 2 from top to bottom.
 * @param number: LED indicator to be controlled.
 * @param color: LED color state as below.
 */
void GLK_SetLEDIndicators(GLKLed_t number, LEDState_t color);

/********************************************************************************
 *              Piezo Buzzer
 *******************************************************************************/

/*
 * Activate Piezo Buzzer: Activates a buzz of specific frequency from the onboard
 * piezo buzzer for a specified length of time.
 * @param frequency: frequency to buzz in Hertz.
 * @param time: Duration of the beep in milliseconds.
 */
void GLK_ActivatePiezoBuzzer(uint16_t frequency, uint16_t time);

/*
 * Set Default Buzzer Beep: Set the frequency and duration of the default beep
 * transmitted when the bell character is transmitted.
 * @param frequency: frequency to buzz in Hertz, default 440Hz.
 * @param duration: Duration of the beep in milliseconds, default 100ms.
 */
void GLK_SetDefaultBuzzerBeep(uint16_t frequency, uint16_t duration);

/* 
 * Set Keypad Buzzer Beep: Set the frequency and duration of the default beep 
 * transmitted when a key is pressed.
 * @param frequency: Frequency of the beep in Hertz, default 0 or off.
 * @param duration: Duration of the beep in milliseconds, default 0 or off.
 */
void GLK_SetKeypadBuzzerBeep(uint16_t frequency, uint16_t duration);

/********************************************************************************
 *              KEYPAD
 *******************************************************************************/

/* Auto Transmit Key Pressed On: Key presses are automatically sent to the host
 * when received by the display. Use this mode for I2C transmissions.
 */
void GLK_AutoTransmitKeyPressedOn();

/*
 * Auto Transmit Key Pressed Off: Key presses are held in the 10 key buffer to be
 * polled by the host using the Poll Key PRess command. Default is Auto Transmit on.
 */
void GLK_AutoTransmitKeyPressedOff();

/*
 * Poll Key Press: Reads the last unread key press from the 10 key display buffer. 
 * If another key is stored in the buffer the MSB will be 1, the MSB will be 0 when
 * the last key press is read. If there are no stored key presses a value of 0 will
 * be returned. Auto transmit key presses must be turned off for this command to be 
 * successful, do not use with I2C.
 * @return Value of key pressed (MSB determines additional keys to be read)
 */
void GLK_PollKeyPress(uint8_t * response);

/*
 * Clear Key Buffer: Clears all key presses from the key buffer.
 */
void GLK_ClearKeyBuffer();

/*
 * Set Debounce Time: Sets the time between a key press and a key read by the display.
 * Most switches will bounce when pressed; the debounce time allows the switch to 
 * settle for an accurate read. Default is 8 representing approcimately 52ms.
 * @param time: Debounce increment (debounce time = time * 6.554ms)
 */
void GLK_SetDebounceTime(uint8_t time);

/*
 * Set Auto Repeat Mode: Sets key press repeat mode to typematic or hold. In typematic 
 * mode if a key press is held, by default the key value is transmitted immediately, 
 * then 5 times a second after a 1 second delay. In hold mode, the keyu down value is
 * transmitted once the when pressed, and then the key up value is send when the key is
 * released. Default is typematic.
 * @param mode: 1 for hold mode or 0 for typematic.
 */
void GLK_SetAutoRepeatMode(uint8_t mode);

/*
 * Auto Repeat Mode Off: Turns auto repeat mode off. Default is on (typematic).
 */
void GLK_AutoRepeatModeOff();

/*
 * Assign Keypad Codes: Assings the keyu down and key up values send to the host 
 * when a key press is detected. A key up and key down value must be sent for every key,
 * a value of 255 will leave the unaltered. Defaults are shown below.
 * @param key_down: Key down values.
 * @param key_up: Key up values.
 */
void GLK_AssingKeypadCodes(uint8_t key_down[9], uint8_t key_up[9]);

/*
 * Keypad Backlight Off: Turns the keypad backlight off.
 */
void GLK_KeypadBacklightOff();

/*
 * Set Keypad Brightness: Immediately sets the keypad brightness. On time is set using 
 * the Backlight On Command. Default is 255.
 * @param brightness: Brightness level from 0 (Dim) to 255 (Bright).
 */
void GLK_SetKeypadBrightness(uint8_t brightness);

/*
 * Set Auto Backlight: Set the way the display and keypad backlights respond 
 * when a key is pressed. The options in the tables below allow a keypress
 * to turn on the display and/or keypad backlights after they have timed
 * out or been turned off.
 * @param setting: What portions of the unit light on a keypress, if any, 
 * and if that press is returned.
 */
void GLK_SetAutoBacklight(uint8_t setting);

/*
 * Set Typematic Delay: Sets the delay between the first key press and first 
 * typematic report when a key is held in typematic mode.
 * @param delay: Time key must be held to trigger typematic reports, specified
 * in 100 ms, default is 10 (1s).
 */
void GLK_SetTypematicDelay(uint8_t delay);

/*
 * Set Typematic Interval: Sets the interval between reported key presses when 
 * a key is held in typematic mode.
 * @param interval: Time between key reports, specified in 100 ms increments,
 * default is 2 (200ms).
 */
void GLK_SetTypematicInterval(uint8_t interval);


/********************************************************************************
 *              MISC
 *******************************************************************************/


/**
 * Read Version Number: Causes display to respond with its firmware version number.
 * @param response: Convert to hexadecimal to view major and minor revision number.
 */
void GLK_ReadVersionNumber(uint8_t * response);

/**
 * Read Module Type: Causes display to respond with its module number.
 * @param response: Module number, see Sample Module Type Responses for a partial list.
 */
void GLK_ReadModuleType(uint8_t * response);

#endif
