#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "../Settings.h"
#include "../Drivers/SYSTEM_Driver.h"
#include "../Utils.h"
#include "UART_Controller.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
#define UART_BUFFER   10

/*******************************************************************************
 *          LOCAL FUNCTION DEFINES
 ******************************************************************************/
static bool checkState(UartData_t data);
static void onReadDone(UartData_t data);

/* Event function pointers */
static bool (*putCommand)(Command_t data);

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
bool checkState(UartData_t data) {
    switch(data.status) {
        default:
        case STA_OK             : return true;
        case STA_NOK            : return false;
        case STA_INVALID_START  : return false;
        case STA_INVALID_STOP   : return false;
        case STA_INVALID_SEP    : return false;
        case STA_INVALID_LENGTH : return false;
        case STA_INVALID_COMMAND: return false;
        case STA_INVALID_MESSAGE: return false;
    }
}

void onReadDone(UartData_t data) {
    if (checkState(data)) {
        Command_t command;
        switch (data.command) {
            // Basic
            case C_HANDSHAKE:
                break;

            // Supply stuff
            case C_SET_VOLTAGE:
                command.command = C_SET_VOLTAGE;
                concatinate(data.message[0], data.message[1], &command.data);
                (*putCommand)(command);
                break;
            case C_SET_CURRENT:
                command.command = C_SET_CURRENT;
                concatinate(data.message[0], data.message[1], &command.data);
                (*putCommand)(command);
                break;
            case C_GET_VOLTAGE:

                break;
            case C_GET_CURRENT:

                break;

            // LCD stuff
            case C_LCD_CONTRAST:
                //lcdSetDisplayContrast(uartData.message[0]); // 1 - 50
                command.command = C_LCD_CONTRAST;
                command.data = data.message[0];
                (*putCommand)(command);
                break;
            case C_LCD_BRIGHTNESS:
                //lcdSetDisplayBrightness(uartData.message[0]); // 1 - 8
                command.command = C_LCD_BRIGHTNESS;
                command.data = data.message[0];
                (*putCommand)(command);
                break;
        }
    } 
}


/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/
void uartInit(bool (*onPutCommand)(Command_t data)) {
    uartDriverInit(UART1_BAUD, &onReadDone);
    
    putCommand = onPutCommand;
    uartDriverEnable(true);
}