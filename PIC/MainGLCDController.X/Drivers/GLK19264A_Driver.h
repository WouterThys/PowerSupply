#ifndef GLK19264A_DRIVER_H
#define GLK19264A_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include "../settings.h"
#include "UART_Driver.h"

/********************************************************************************
 *              DEFINES
 *******************************************************************************/

void GLK_Init(void);

void GLK_Write(const char * text);

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
 * @param c: column, value between 1 and number of character colums.
 * @param r: row, Value between 1 and numer of character rows.
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
 * Initialize Label: Designates a protion of the screen that can be easily updated with on eline of text,
 * often used to display variables.
 * @param id: Unique label identification number, value between 0 and 15.
 * @param x1: Leftmost coordinate.
 * @param y1: Topmost coordinate.
 * @param x2: Rightmost coordinate.
 * @param y2: Bottommost corrdinate.
 * @param vert: Vertical justification of the text; o for top, 1 for middle, or 2 for bottom.
 * @param hor: Horizontal justification of the label text; 0 for left, 1 for centre, or 2 for right.
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
 * @param dir: Direction of the scrolling behaviour; 0 for left, 1 for right, or 2 for bounce.
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


#endif
