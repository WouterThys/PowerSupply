#ifndef GLK19264A_DRIVER_H
#define GLK19264A_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include "../settings.h"
#include "UART_Driver.h"

/********************************************************************************
 *              ENUMS
 *******************************************************************************/
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

/********************************************************************************
 *              MY FUNCTIONS
 *******************************************************************************/

void GLK_Init(void);

void GLK_Write(const char * text);


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
