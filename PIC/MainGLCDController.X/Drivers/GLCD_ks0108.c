#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <xc.h>

#include "GLCD_ks0108.h"



/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
// Name defines
#define EN_DELAY()  __delay_us(5);
// Glcd macros
#define INPUT_MODE  0xFF
#define OUTPUT_MODE 0x00

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/
#define _BV(i)      (1 << (i))
#define IsFixedWidthFont(font)  (GLCD_FontRead(font+FONT_LENGTH) == 0 && GLCD_FontRead(font+FONT_LENGTH+1) == 0)

#define GLCD_Enable()\
do{\
    EN_DELAY();\
    FastWriteHigh(EN);\
    EN_DELAY();\
    FastWriteLow(EN);\
    EN_DELAY();\
}while(0)

#define GLCD_SelectChip(chip)\
do{\
    switch(chip){\
        case 0: CSEL1=0; CSEL2=1; CSEL3=1;break;\
        case 1: CSEL1=1; CSEL2=0; CSEL3=1;break;\
        case 2: CSEL1=1; CSEL2=1; CSEL3=0;break;\
    }\
}while(0)

#define GLCD_WaitReady(chip)\
do{\
    GLCD_SelectChip(chip);\
    GlcdDataDir(INPUT_MODE);\
    FastWriteLow(D_I);\
    FastWriteHigh(R_W);\
    FastWriteHigh(EN);\
    EN_DELAY();\
    while(GLCD_DIN_REG & GLCD_BUSY_FLAG);\
    FastWriteLow(EN);\
}while(0)

#define GLCD_ReadData(data)\
do{\
    GLCD_DoReadData(1);\
    data = GLCD_DoReadData(0);\
}while(0)

#define GLCD_WriteCommand(cmd, chip)\
do{\
    GLCD_WaitReady(chip);\
    FastWriteLow(D_I);\
    FastWriteLow(R_W);\
    GlcdDataDir(OUTPUT_MODE);\
    \
    EN_DELAY();\
    GlcdDataOut(cmd);\
    GLCD_Enable();\
    EN_DELAY();\
    EN_DELAY();\
    GlcdDataOut(0x00);\
}while(0)

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
// Control variables
static GlcdCoord GLCD_Coord;
static bool GLCD_Inverted;
static FontCallBack GLCD_FontRead;
static uint8_t GLCD_FontColor;
static const uint8_t* GLCD_Font;

// Private functions
uint8_t GLCD_DoReadData(uint8_t first);
void GLCD_WriteData(uint8_t data);

//#define GLCD_DEBUG    // Uncomment for slow drawing in debug mode

/*******************************************************************************
 *          BASIC FUNCTIONS
 ******************************************************************************/
void GLCD_Init(bool invert) {
    TRISD = 0x00;
    TRISB = 0x03;

    __delay_ms(10);
    __delay_ms(10);
    __delay_ms(10);
    __delay_ms(10);
    __delay_ms(10);
    __delay_ms(10);

    // Default control pins
    FastWriteLow(D_I);
    FastWriteLow(R_W);
    FastWriteLow(EN);

    // Reset module
    FastWriteLow(RST);
    __delay_ms(10);
    __delay_ms(10);
    __delay_ms(10);
    __delay_ms(10);
    __delay_ms(10);
    __delay_ms(10);
    FastWriteHigh(RST);
    __delay_ms(10);
    __delay_ms(10);
    __delay_ms(10);
    __delay_ms(10);
    __delay_ms(10);
    __delay_ms(10);

    // Initial coordinates are the left upper corner (0,0)
    GLCD_Coord.x = 0;
    GLCD_Coord.y = 0;
    GLCD_Coord.page = 0;

    // Choose inverted printing style (white with black dots)
    GLCD_Inverted = invert;

    uint8_t chip;
    for (chip = 0; chip < DISPLAY_WIDTH / CHIP_WIDTH; chip++) {
        GLCD_WriteCommand(GLCD_ON, chip); // Display ON
        GLCD_WriteCommand(GLCD_DISP_START, chip); // Set Display start line at 0
    }

    GLCD_ClearScreen(invert ? BLACK : WHITE); // White or Black screen clearing
    GLCD_GotoXY(0, 0);
    __delay_ms(10);
    __delay_ms(10);
    __delay_ms(10);
    __delay_ms(10);
    __delay_ms(10);
}

void GLCD_GotoXY(uint8_t x, uint8_t y) {
    uint8_t chip, cmd;
    //    if ((x > DISPLAY_WIDTH - 1) || (y > DISPLAY_HEIGHT - 1))
    //        return; // Coordinates are not valid

    if ((x > DISPLAY_WIDTH - 1)) {
        x = 0;
        y = y + 8;
    }
    if (y > DISPLAY_HEIGHT - 1) {
        return;
    }

    // Save the new coordinates
    GLCD_Coord.x = x;
    GLCD_Coord.y = y;

    if (y / 8 != GLCD_Coord.page) {
        GLCD_Coord.page = y / 8;
        // Set y address on all chips
        cmd = GLCD_SET_PAGE | GLCD_Coord.page;
        for (chip = 0; chip < DISPLAY_WIDTH / CHIP_WIDTH; chip++) {
            GLCD_WriteCommand(cmd, chip);
        }
    }

    chip = GLCD_Coord.x / CHIP_WIDTH;
    x = x % CHIP_WIDTH;
    cmd = GLCD_SET_ADD | x;
    GLCD_WriteCommand(cmd, chip); // Set x address on active chip
}

void GLCD_ClearPage(uint8_t page, uint8_t color) {
    for (uint8_t x = 0; x < DISPLAY_WIDTH; x++) {
        GLCD_GotoXY(x, page * 8);
        GLCD_WriteData(color);
    }
}

void GLCD_ClearScreen(uint8_t color) {
    uint8_t page;
    for (page = 0; page < 8; page++) {
        GLCD_GotoXY(0, page * 8);
        GLCD_ClearPage(page, color);
    }
}

void GLCD_SetInverted(bool invert) { // changed type to boolean
    if (GLCD_Inverted != invert) {
        GLCD_InvertRect(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);
        GLCD_Inverted = invert;
    }
}

uint8_t GLCD_DoReadData(uint8_t first) {
    uint8_t data, chip;
    chip = GLCD_Coord.x / CHIP_WIDTH;
    GLCD_WaitReady(chip);
    if (first) {
        if (GLCD_Coord.x % CHIP_WIDTH == 0 && chip > 0) { // todo , remove this test and call GotoXY always?
            GLCD_GotoXY(GLCD_Coord.x, GLCD_Coord.y);
            GLCD_WaitReady(chip);
        }
    }
    FastWriteHigh(D_I); // D/I = 1
    FastWriteHigh(R_W); // R/W = 1

    FastWriteHigh(EN);
    EN_DELAY();

    data = GLCD_DIN_REG;

    FastWriteLow(EN);
    if (first == 0)
        GLCD_GotoXY(GLCD_Coord.x, GLCD_Coord.y);
    if (GLCD_Inverted)
        data = ~data;
    return data;
}

void GLCD_WriteData(uint8_t data) {
    uint8_t displayData, yOffset, chip;

#ifdef GLCD_DEBUG
    volatile uint16_t i;
    for (i = 0; i < 5000; i++);
#endif

    if (GLCD_Coord.x >= DISPLAY_WIDTH)
        return;
    chip = GLCD_Coord.x / CHIP_WIDTH;
    GLCD_WaitReady(chip);


    if (GLCD_Coord.x % CHIP_WIDTH == 0 && chip > 0) { // todo , ignore address 0???
        GLCD_GotoXY(GLCD_Coord.x, GLCD_Coord.y);
    }

    FastWriteHigh(D_I); // D/I = 1
    FastWriteLow(R_W); // R/W = 0
    GlcdDataDir(OUTPUT_MODE); // data port is output

    yOffset = GLCD_Coord.y % 8;

    if (yOffset != 0) {
        // first page
        GLCD_ReadData(data);
        FastWriteHigh(D_I); // D/I = 1
        FastWriteLow(R_W); // R/W = 0
        GLCD_SelectChip(chip);
        GlcdDataDir(OUTPUT_MODE); // data port is output

        displayData |= data << yOffset;
        if (GLCD_Inverted)
            displayData = ~displayData;
        GlcdDataOut(displayData); // write data
        GLCD_Enable(); // enable

        // second page
        GLCD_GotoXY(GLCD_Coord.x, GLCD_Coord.y + 8);

        GLCD_ReadData(displayData);


        FastWriteHigh(D_I); // D/I = 1
        FastWriteLow(R_W); // R/W = 0
        GLCD_SelectChip(chip);

        GlcdDataDir(OUTPUT_MODE); // data port is output

        displayData |= data >> (8 - yOffset);
        if (GLCD_Inverted)
            displayData = ~displayData;
        GlcdDataOut(displayData); // write data
        GLCD_Enable(); // enable

        GLCD_GotoXY(GLCD_Coord.x + 1, GLCD_Coord.y - 8);
    } else {
        // just this code gets executed if the write is on a single page
        if (GLCD_Inverted)
            data = ~data;
        EN_DELAY();
        GlcdDataOut(data); // write data
        GLCD_Enable(); // enable
        GLCD_Coord.x++;
    }
}

/*******************************************************************************
 *          SPECIAL FUNCTIONS
 ******************************************************************************/

void GLCD_SetDot(uint8_t x, uint8_t y, uint8_t color) {
    uint8_t data;
    // Read data from display memory
    GLCD_GotoXY(x, y - y % 8);
    GLCD_ReadData(data);

    if (color == BLACK) {
        data |= 0x01 << (y % 8); // set dot
    } else {
        data &= ~(0x01 << (y % 8)); // clear dot
    }
    GLCD_WriteData(data); // write data back to display
}

// Extra draw line macros:
#define _GLCD_AbsDiff(a,b)  ((a>b) ? (x-y):(y-x))
#define _GLCD_Swap(a,b) \
do\
{\
uint8_t t;\
	t=a;\
	a=b;\
	b=t;\
} while(0)

void GLCD_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color) {
    uint8_t dx, dy, x, y, steep;
    int8_t error, ystep;

    steep = _GLCD_AbsDiff(y1, y2) > _GLCD_AbsDiff(x1, x2);
    if (steep) {
        _GLCD_Swap(x1, y1);
        _GLCD_Swap(x2, y2);
    }
    if (x1 > x2) {
        _GLCD_Swap(x1, x2);
        _GLCD_Swap(y1, y2);
    }

    dx = x2 - x1;
    dy = _GLCD_AbsDiff(y2, y1);
    error = dx / 2;
    y = y1;
    if (y1 < y2) ystep = 1;
    else ystep = -1;

    for (x = x1; x <= x2; x++) {
        if (steep) GLCD_SetDot(y, x, color);
        else GLCD_SetDot(x, y, color);
        error = error - dy;
        if (error < 0) {
            y = y + ystep;
            error = error + dx;
        }
    }
}

void GLCD_DrawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color) {
    GLCD_DrawHoriLine(x, y, width); // top
    GLCD_DrawHoriLine(x, y + height, width); // bottom
    GLCD_DrawVertLine(x, y, height); // left
    GLCD_DrawVertLine(x + width, y, height); // right
}

void GLCD_DrawRoundRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t radius, uint8_t color) {
    int16_t tSwitch, x1 = 0, y1 = radius;
    tSwitch = 3 - 2 * radius;

    while (x1 <= y1) {
        GLCD_SetDot(x + radius - x1, y + radius - y1, color);
        GLCD_SetDot(x + radius - y1, y + radius - x1, color);

        GLCD_SetDot(x + width - radius + x1, y + radius - y1, color);
        GLCD_SetDot(x + width - radius + y1, y + radius - x1, color);

        GLCD_SetDot(x + width - radius + x1, y + height - radius + y1, color);
        GLCD_SetDot(x + width - radius + y1, y + height - radius + x1, color);

        GLCD_SetDot(x + radius - x1, y + height - radius + y1, color);
        GLCD_SetDot(x + radius - y1, y + height - radius + x1, color);

        if (tSwitch < 0) {
            tSwitch += (4 * x1 + 6);
        } else {
            tSwitch += (4 * (x1 - y1) + 10);
            y1--;
        }
        x1++;
    }

    GLCD_DrawHoriLine(x + radius, y, width - (2 * radius)); // top
    GLCD_DrawHoriLine(x + radius, y + height, width - (2 * radius)); // bottom
    GLCD_DrawVertLine(x, y + radius, height - (2 * radius)); // left
    GLCD_DrawVertLine(x + width, y + radius, height - (2 * radius)); // right
}

void GLCD_FillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color) {
    uint8_t mask, pageOffset, h, i, data;
    height++;

    pageOffset = y % 8;
    y -= pageOffset;
    mask = 0xFF;
    if (height < 8 - pageOffset) {
        mask >>= (8 - height);
        h = height;
    } else {
        h = 8 - pageOffset;
    }
    mask <<= pageOffset;

    GLCD_GotoXY(x, y);
    for (i = 0; i <= width; i++) {
        GLCD_ReadData(data);

        if (color == BLACK) {
            data |= mask;
        } else {
            data &= ~mask;
        }

        GLCD_WriteData(data);
    }

    while (h + 8 <= height) {
        h += 8;
        y += 8;
        GLCD_GotoXY(x, y);

        for (i = 0; i <= width; i++) {
            GLCD_WriteData(color);
        }
    }

    if (h < height) {
        mask = ~(0xFF << (height - h));
        GLCD_GotoXY(x, y + 8);

        for (i = 0; i <= width; i++) {
            GLCD_ReadData(data);

            if (color == BLACK) {
                data |= mask;
            } else {
                data &= ~mask;
            }

            GLCD_WriteData(data);
        }
    }
}

void GLCD_InvertRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
    uint8_t mask, pageOffset, h, i, data, tmpData;
    height++;

    pageOffset = y % 8;
    y -= pageOffset;
    mask = 0xFF;
    if (height < 8 - pageOffset) {
        mask >>= (8 - height);
        h = height;
    } else {
        h = 8 - pageOffset;
    }
    mask <<= pageOffset;

    GLCD_GotoXY(x, y);
    for (i = 0; i <= width; i++) {
        GLCD_ReadData(data);
        tmpData = ~data;
        data = (tmpData & mask) | (data & ~mask);
        GLCD_WriteData(data);
    }

    while (h + 8 <= height) {
        h += 8;
        y += 8;
        GLCD_GotoXY(x, y);

        for (i = 0; i <= width; i++) {
            GLCD_ReadData(data);
            GLCD_WriteData(~data);
        }
    }

    if (h < height) {
        mask = ~(0xFF << (height - h));
        GLCD_GotoXY(x, y + 8);

        for (i = 0; i <= width; i++) {
            GLCD_ReadData(data);
            tmpData = ~data;
            data = (tmpData & mask) | (data & ~mask);
            GLCD_WriteData(data);
        }
    }
}

//
// Font Functions
//

uint8_t ReadPgmData(const uint8_t* ptr) {
    return *ptr;
}

void GLCD_SelectFontEx(const uint8_t* font, uint8_t color, FontCallBack callback) {
    GLCD_Font = font;
    GLCD_FontRead = callback;
    GLCD_FontColor = color;
}

void GLCD_PrintNumber(long n) {
    byte buf[10]; // prints up to 10 digits
    byte i = 0;
    if (n == 0)
        GLCD_PutChar('0');
    else {
        if (n < 0) {
            GLCD_PutChar('-');
            n = -n;
        }
        while (n > 0 && i <= 10) {
            buf[i++] = n % 10; // n % base
            n /= 10; // n/= base
        }
        for (; i > 0; i--)
            GLCD_PutChar((char) (buf[i - 1] < 10 ? '0' + buf[i - 1] : 'A' + buf[i - 1] - 10));
    }
}

void GLCD_PrintHexNumber(uint16_t n) {
    int8_t i;
    uint8_t d, previous_write = 0;

    if (n == 0) {
        GLCD_PutChar('0');
        return;
    }

    for (i = 3; i >= 0; i--) {
        d = ((0x0F << i * 4) & n) >> i * 4;

        if (d < 0xA)
            d += '0';
        else
            d = (d - 0xA) + 'A';

        if (d != '0' || previous_write) {
            GLCD_PutChar((char) d);
            previous_write = 1;
        }
    }
}

void GLCD_PrintRealNumber(double n) {
    long int_part, frac_part;
    uint8_t sign = 0;

    if (n < 0) {
        sign = 1;
        n *= -1;
    }

    int_part = (long) n;
    frac_part = (long) ((n - (double) int_part)*1000000);
    if (sign) GLCD_PutChar('-');
    GLCD_PrintNumber(int_part);
    GLCD_PutChar('.');

    while ((frac_part != 0) && (frac_part % 10 == 0)) {
        frac_part /= 10;
    }

    GLCD_PrintNumber(frac_part);
}

int GLCD_PutChar(char c) {
    if (GLCD_Font == 0)
        return 0; // no font selected

    uint8_t width = 0;
    uint8_t height = GLCD_FontRead(GLCD_Font + FONT_HEIGHT);
    uint8_t bytes = (height + 7) / 8; /* calculates height in rounded up bytes */

    uint8_t firstChar = GLCD_FontRead(GLCD_Font + FONT_FIRST_CHAR);
    uint8_t charCount = GLCD_FontRead(GLCD_Font + FONT_CHAR_COUNT);

    uint16_t index = 0;
    uint8_t x = GLCD_Coord.x, y = GLCD_Coord.y;
    uint8_t thielefont;

    if (c < firstChar || c >= (firstChar + charCount)) {
        return 0; // invalid char
    }
    c -= firstChar;

    if (IsFixedWidthFont(GLCD_Font)) {
        thielefont = 0;
        width = GLCD_FontRead(GLCD_Font + FONT_FIXED_WIDTH);
        index = c * bytes * width + FONT_WIDTH_TABLE;
    } else {
        // variable width font, read width data, to get the index
        thielefont = 1;
        /*
         * Because there is no table for the offset of where the data
         * for each character glyph starts, run the table and add up all the
         * widths of all the characters prior to the character we
         * need to locate.
         */
        for (uint8_t i = 0; i < c; i++) {
            index += GLCD_FontRead(GLCD_Font + FONT_WIDTH_TABLE + i);
        }
        /*
         * Calculate the offset of where the font data
         * for our character starts.
         * The index value from above has to be adjusted because
         * there is potentialy more than 1 byte per column in the glyph,
         * when the characgter is taller than 8 bits.
         * To account for this, index has to be multiplied
         * by the height in bytes because there is one byte of font
         * data for each vertical 8 pixels.
         * The index is then adjusted to skip over the font width data
         * and the font header information.
         */

        index = index * bytes + charCount + FONT_WIDTH_TABLE;

        /*
         * Finally, fetch the width of our character
         */
        width = GLCD_FontRead(GLCD_Font + FONT_WIDTH_TABLE + c);
    }

    // last but not least, draw the character

    /*
     * Paint font data bits and write them to LCD memory 1 LCD page at a time.
     * This is very different from simply reading 1 byte of font data
     * and writing all 8 bits to LCD memory and expecting the write data routine
     * to fragement the 8 bits across LCD 2 memory pages when necessary.
     * That method (really doesn't work) and reads and writes the same LCD page
     * more than once as well as not do sequential writes to memory.
     *
     * This method of rendering while much more complicated, somewhat scrambles the font
     * data reads to ensure that all writes to LCD pages are always sequential and a given LCD
     * memory page is never read or written more than once.
     * And reads of LCD pages are only done at the top or bottom of the font data rendering
     * when necessary.
     * i.e it ensures the absolute minimum number of LCD page accesses
     * as well as does the sequential writes as much as possible.
     *
     */

    uint8_t pixels = height + 1; /* 1 for gap below character*/
    uint8_t p;
    uint8_t dy;
    uint8_t tfp;
    uint8_t dp;
    uint8_t dbyte;
    uint8_t fdata;

    for (p = 0; p < pixels;) {
        dy = y + p;

        /*
         * Align to proper Column and page in LCD memory
         */

        GLCD_GotoXY(x, (dy & ~7));

        uint16_t page = p / 8 * width; // page must be 16 bit to prevent overflow

        for (uint8_t j = 0; j < width; j++) /* each column of font data */ {

            /*
             * Fetch proper byte of font data.
             * Note:
             * This code "cheats" to add the horizontal space/pixel row
             * below the font.
             * It essentially creates a font pixel of 0 when the pixels are
             * out of the defined pixel map.
             *
             * fake a fondata read read when we are on the very last
             * bottom "pixel". This lets the loop logic continue to run
             * with the extra fake pixel. If the loop is not the
             * the last pixel the pixel will come from the actual
             * font data, but that is ok as it is 0 padded.
             *
             */

            if (p >= height) {
                /*
                 * fake a font data read for padding below character.
                 */
                fdata = 0;
            } else {
                fdata = GLCD_FontRead(GLCD_Font + index + page + j);

                /*
                 * Have to shift font data because Thiele shifted residual
                 * font bits the wrong direction for LCD memory.
                 *
                 * The real solution to this is to fix the variable width font format to
                 * not shift the residual bits the wrong direction!!!!
                 */
                if (thielefont && (height - (p&~7)) < 8) {
                    fdata >>= 8 - (height & 7);
                }
            }

            if (GLCD_FontColor == WHITE)
                fdata ^= 0xff; /* inverted data for "white" font color	*/


            /*
             * Check to see if a quick full byte write of font
             * data can be done.
             */

            if (!(dy & 7) && !(p & 7) && ((pixels - p) >= 8)) {
                /*
                 * destination pixel is on a page boundary
                 * Font data is on byte boundary
                 * And there are 8 or more pixels left
                 * to paint so a full byte write can be done.
                 */

                GLCD_WriteData(fdata);
                continue;
            } else {
                /*
                 * No, so must fetch byte from LCD memory.
                 */
                GLCD_ReadData(dbyte);
            }

            /*
             * At this point there is either not a full page of data
             * left to be painted  or the font data spans multiple font
             * data bytes. (or both) So, the font data bits will be painted
             * into a byte and then written to the LCD memory.page.
             */


            tfp = p; /* font pixel bit position 		*/
            dp = dy & 7; /* data byte pixel bit position */

            /*
             * paint bits until we hit bottom of page/byte
             * or run out of pixels to paint.
             */
            while ((dp <= 7) && (tfp) < pixels) {
                if (fdata & _BV(tfp & 7)) {
                    dbyte |= _BV(dp);
                } else {
                    dbyte &= ~_BV(dp);
                }

                /*
                 * Check for crossing font data bytes
                 */
                if ((tfp & 7) == 7) {
                    fdata = GLCD_FontRead(GLCD_Font + index + page + j + width);

                    /*
                     * Have to shift font data because Thiele shifted residual
                     * font bits the wrong direction for LCD memory.
                     *
                     */

                    if ((thielefont) && ((height - tfp) < 8)) {
                        fdata >>= (8 - (height & 7));
                    }

                    if (GLCD_FontColor == WHITE)
                        fdata ^= 0xff; /* inverted data for "white" color	*/
                }
                tfp++;
                dp++;
            }

            /*
             * Now flush out the painted byte.
             */
            GLCD_WriteData(dbyte);
        }

        /*
         * now create a horizontal gap (vertical line of pixels) between characters.
         * Since this gap is "white space", the pixels painted are oposite of the
         * font color.
         *
         * Since full LCD pages are being written, there are 4 combinations of filling
         * in the this gap page.
         *	- pixels start at bit 0 and go down less than 8 bits
         *	- pixels don't start at 0 but go down through bit 7
         *	- pixels don't start at 0 and don't go down through bit 7 (fonts shorter than 6 hi)
         *	- pixels start at bit 0 and go down through bit 7 (full byte)
         *
         * The code below creates a mask of the bits that should not be painted.
         *
         * Then it is easy to paint the desired bits since if the color is WHITE,
         * the paint bits are set, and if the coloer is not WHITE the paint bits are stripped.
         * and the paint bits are the inverse of the desired bits mask.
         */



        if ((dy & 7) || (pixels - p < 8)) {
            uint8_t mask = 0;

            GLCD_ReadData(dbyte);

            if (dy & 7)
                mask |= _BV(dy & 7) - 1;

            if ((pixels - p) < 8)
                mask |= ~(_BV(pixels - p) - 1);


            if (GLCD_FontColor == WHITE)
                dbyte |= ~mask;
            else
                dbyte &= mask;

        } else {
            if (GLCD_FontColor == WHITE)
                dbyte = 0xff;
            else
                dbyte = 0;
        }

        GLCD_WriteData(dbyte);

        /*
         * advance the font pixel for the pixels
         * just painted.
         */

        p += 8 - (dy & 7);
    }


    /*
     * Since this rendering code always starts off with a GotoXY() it really isn't necessary
     * to do a real GotoXY() to set the h/w location after rendering a character.
     * We can get away with only setting the s/w version of X & Y.
     *
     * Since y didn't change while rendering, it is still correct.
     * But update x for the pixels rendered.
     *
     */

    GLCD_GotoXY(x + width + 1, y);

    return 1; // valid char
}

void GLCD_Puts(const char *str) {
    int x = GLCD_Coord.x;
    while (*str != 0) {
        if (*str == '\n') {
            GLCD_GotoXY(x, GLCD_Coord.y + GLCD_FontRead(GLCD_Font + FONT_HEIGHT));
        } else {
            GLCD_PutChar(*str);
        }
        str++;
    }
}

uint8_t GLCD_CharWidth(char c) {
    uint8_t width = 0;
    uint8_t firstChar = GLCD_FontRead(GLCD_Font + FONT_FIRST_CHAR);
    uint8_t charCount = GLCD_FontRead(GLCD_Font + FONT_CHAR_COUNT);

    // read width data
    if (c >= firstChar && c < (firstChar + charCount)) {
        c -= firstChar;
        width = GLCD_FontRead(GLCD_Font + FONT_WIDTH_TABLE + c) + 1;
    }

    return width;
}

uint16_t GLCD_StringWidth(const char *str) {
    uint16_t width = 0;

    while (*str != 0) {
        width += GLCD_CharWidth(*str++);
    }

    return width;
}

void GLCD_CursorTo(uint8_t x, uint8_t y) { // 0 based coordinates for fixed width fonts (i.e. systemFont5x7)
    GLCD_GotoXY(x * (GLCD_FontRead(GLCD_Font + FONT_FIXED_WIDTH) + 1),
            y * (GLCD_FontRead(GLCD_Font + FONT_HEIGHT) + 1));
}

void GLCD_DrawBitmap(const uint8_t * bitmap, uint8_t x, uint8_t y, uint8_t color) {
    uint8_t width, height;
    uint8_t i, j;

    width = ReadPgmData(bitmap++);
    height = ReadPgmData(bitmap++);
    for (j = 0; j < height / 8; j++) {
        GLCD_GotoXY(x, y + (j * 8));
        for (i = 0; i < width; i++) {
            uint8_t displayData = ReadPgmData(bitmap++);
            if (color == BLACK)
                GLCD_WriteData(displayData);
            else
                GLCD_WriteData(~displayData);
        }
    }
}
