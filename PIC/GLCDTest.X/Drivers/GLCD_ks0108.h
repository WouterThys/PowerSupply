/* 
 * File:   GLCD_ks0108.h
 * Author: wouter
 *
 * Created on 20 mei 2015, 16:10
 */

#include <stdint.h>
#include <stdbool.h>
#include "../settings.h"

#ifndef GLCD_KS0108_H
#define	GLCD_KS0108_H

/*******************************************************************************
*           DEFINES
*******************************************************************************/


// macros to fast write data to pins known at compile time
#define FastWriteHigh(_pin_)	_pin_ = 1
#define FastWriteLow(_pin_) 	_pin_ = 0

#define DISPLAY_WIDTH   192
#define DISPLAY_HEIGHT  64

#define CHIP_WIDTH      64      // Pixels per chip

#define GlcdDataOut(d)  GLCD_DOUT_REG = d
#define GlcdDataDir(d)  GLCD_DDIR_REG = d

// Commands
#define GLCD_ON         ((uint8_t)0x3F)
#define GLCD_OFF        ((uint8_t)0x3E)
#define GLCD_DISP_START ((uint8_t)0xC0)

#define GLCD_SET_ADD    ((uint8_t)0x40)
#define GLCD_SET_PAGE   ((uint8_t)0xB8)

#define GLCD_BUSY_FLAG  ((uint8_t)0x80)

// Colors
#define BLACK           ((uint8_t)0x00)
#define WHITE           ((uint8_t)0xFF)

// User Constants
#define NON_INVERTED    false
#define INVERTED        true

// Font Indices
#define FONT_LENGTH         0
#define FONT_FIXED_WIDTH	2
#define FONT_HEIGHT         3
#define FONT_FIRST_CHAR		4
#define FONT_CHAR_COUNT		5
#define FONT_WIDTH_TABLE	6

// Coordinate struct
typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t page;
} GlcdCoord;

typedef uint8_t (*FontCallBack)(const uint8_t*);
typedef uint8_t byte;

uint8_t ReadPgmData(const uint8_t* ptr);        //Standard Read Callback


/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/



/*******************************************************************************
*           FUNCTIONS
*******************************************************************************/
#define GLCD_DrawVertLine(x, y, length, color) GLCD_FillRect(x, y, 0, length, color)
#define GLCD_DrawHoriLine(x, y, length, color) GLCD_FillRect(x, y, length, 0, color)
#define GLCD_DrawCircle(xCenter, yCenter, radius, color) GLCD_DrawRoundRect(xCenter-radius, yCenter-radius, 2*radius, 2*radius, radius, color)
#define GLCD_ClearScreenX() GLCD_FillRect(70, 9, 117, 50, BLACK)
#define GLCD_ClearSysTextLine(_line) GLCD_FillRect(0, (line*8), (DISPLAY_WIDTH-1), ((line*8)+ 7), WHITE )
#define GLCD_SelectFont(font) GLCD_SelectFontEx(font, BLACK, ReadPgmData)

// Control functions
/**
 * Initialises all the parameters to the default setting, as well as writing the
 * tri-state registers. Resets all chips and clears (fills) the screen
 * @param invert: Black or White pixel clearing
 */
void GLCD_Init(bool invert);
/**
 * Go to a specified x and y location on the screen, selecting the wright chip
 * and page. Returns from the function when coordinates are invalid.
 * @param x: X (Horizontal) location  on the screen
 * @param y: Y (Vertical) location  on the screen
 */
void GLCD_GotoXY(uint8_t x, uint8_t y);
/**
 * Clear one page in slected color.
 * @param page: Page (8bit vertical block) to clear
 * @param color: Black or White pixel clearing
 */
void GLCD_ClearPage(uint8_t page, uint8_t color);
/**
 * Clear the full screan
 * @param color: Black or White pixel clearing
 */
void GLCD_ClearScreen(uint8_t color);

// Graphic Functions
/**
 * Draw a line (one pixel thickness) from 2 specified points inside the screen.
 * @param x1: first horizontal position
 * @param y1: first vertical position
 * @param x2: second horizontal position
 * @param y2: second vertical position
 * @param color: BLACK or WHITE
 */
void GLCD_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);
void GLCD_DrawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);
void GLCD_DrawRoundRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t radius, uint8_t color);
void GLCD_FillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);
void GLCD_InvertRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
/**
 * Invert the screen
 * @param invert: INVERTED or NON_INVERTED
 */
void GLCD_SetInverted(bool invert);
/**
 * Place one pixel on the specified location on the screen. Select the color
 * Black or White to fill or clear a pixel.
 * @param x: Horizontal location
 * @param y: Vertical location
 * @param color: Black or White pixel
 */
void GLCD_SetDot(uint8_t x, uint8_t y, uint8_t color);
void GLCD_DrawBitmap(const uint8_t *bitmap, uint8_t x, uint8_t y, uint8_t color);

// Font Functions
void GLCD_SelectFontEx(const uint8_t* font, uint8_t color, FontCallBack callback);
int  GLCD_PutChar(char c);
void GLCD_Puts(const char *str);
void GLCD_PrintNumber(long n);
void GLCD_PrintHexNumber(uint16_t n);
void GLCD_PrintRealNumber(double n);
void GLCD_CursorTo( uint8_t x, uint8_t y); // 0 based coordinates for fixed width fonts (i.e. systemFont5x7)

uint8_t  GLCD_CharWidth(char c);
uint16_t GLCD_StringWidth(const char *str);

/*******************************************************************************
*           GLCD COMMANDS
*******************************************************************************/

#endif	/* GLCD_KS0108_H */

