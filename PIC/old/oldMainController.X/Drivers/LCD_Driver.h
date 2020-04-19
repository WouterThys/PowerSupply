#ifndef LCD_DRIVER_H
#define	LCD_DRIVER_H

#ifdef	__cplusplus
extern "C" {
#endif


/**
 * Initialize the LCD
 */
void lcdInit();    

/**
 * 
 * @param c
 */
void lcdWriteChar(char c);

void lcdWriteDigit(uint16_t d);

/**
 * 
 * @param str
 */
void lcdWriteString(const char *str);

void lcdWriteInt(int16_t value);

void lcdWriteDouble(double value, uint16_t precision);
    
    
/**
 * This command sets the I2C address. The address must be an even number (LSB = 0).  The 
 * address change requires 20 microseconds to take effect; therefore, the subsequent input 
 * must have an appropriate delay.  The default I2C address can be restored if SPI or RS-232
 * is selected as the communication mode.
 * Default: 0x50
 * @param address
 */
void lcdChangeI2CAddress(uint8_t address);

/**
 * This command sets the RS-232 BAUD rate. The single byte parameter selects the desired 
 * BAUD rate as in the table below.  The new BAUD rate requires 20 microseconds to take 
 * effect;  therefore,  the  subsequent  input  must  have  an  appropriate  delay.    The  default  
 * BAUD  rate  can  be  restored  if  I2C  or  SPI  is  selected  as  the  communication  mode.    Illegal  
 * parameter input will be discarded.
 * @param baud
 */
void lcdChangeBaudRate(uint8_t baud);

/**
 * This command turns on/off the LCD display screen. The display text is not altered.
 * Default:  LCD screen is on
 * @param on
 */
void lcdTurnDisplayOn(bool on);

/**
 * This command moves the cursor to a specified location where the next character will be 
 * displayed.  The typical cursor position for a 2-line 16-character display is show below; 
 * a cursor position outside these ranges will not be viewable.
 * @param line
 * @param x
 */
void lcdSetCursorPosition(uint8_t line, uint8_t x);

/**
 * This command moves the cursor to line 1, column 1 of the LCD screen. The display text is 
 * not altered.
 * Default:  None
 */
void lcdSetCursorHome();

/**
 * This command turns on/off the underline cursor.
 * Default:  Underline cursor is off
 * @param on
 */
void lcdCursorUnderlineOn(bool on);

/**
 * This command moves the cursor position left 1 space whether the cursor is turned on or not. 
 * The displayed character is not altered. 
 * Default: None
 */
void lcdMoveCursorLeft();

/**
 * This command moves the cursor position left 1 space whether the cursor is turned on or not. 
 * The displayed character is not altered. 
 * Default: None
 */
void lcdMoveCursorRight();

/**
 * This command turns on/off the blinking cursor.  
 * Default:  The blinking cursor is off
 * @param on
 */
void lcdTurnOnBlinkingCursor(bool on);

/**
 * This  command  is  destructive  backspace.  The  cursor  is  moved  back  one  space  and  the  
 * character on the cursor is deleted.
 * Default:  None
 */
void lcdBackspace();

/**
 * This command clears the entire display and place the cursor at line 1 column 1.
 * Default:  None
 */
void lcdClearScreen();

/**
 * This command sets the display contrast. The contrast setting can be between 1 and 50, 
 * where 50 is the highest contrast.
 * Default:  40
 * @param contrast
 */
void lcdSetDisplayContrast(uint8_t contrast);

/**
 * This command sets the back-light brightness level. The value can be between 1 and 8. 
 * Default: 8 
 * @param brightness
 */
void lcdSetDisplayBrightness(uint8_t brightness);

/**
 * There is space for eight user-defined custom characters. This command loads the custom 
 * character  into  one  of  the  eight  locations.  The  custom  character  pattern  is  bit  mapped  
 * into  8  data  bytes. The bit map for Spanish character  ?¿?  is  shown  in  
 * table  below. To display the custom character, user has to enter the address of the character (0 to 8).
 * @param character
 */
void lcdLoadCustomChar(uint8_t address, const char * character);

/**
 * This command shifts the display to the left 1 space. The cursor position also moves with 
 * the display, and the display data is not altered.
 * Default: None
 */
void lcdShiftDisplayLeft();

/**
 * This command shifts the display to the right 1 space. The cursor position also moves with 
 * the display, and the display data is not altered.
 * Default: None
 */
void lcdShiftDisplayRight();

/**
 * This command displays the firmware version.
 * Default:  None
 */
void lcdDisplayFirmwareVersion();

/**
 * This command displays the RS-232 BAUD rate.
 * Default:  None
 */
void lcdDisplayBaudRate();

/**
 * This command displays the current I2C slave address.
 * Default: None
 */
void lcdDisplayI2CAddress();


#ifdef	__cplusplus
}
#endif

#endif	/* LCD_DRIVER_H */

