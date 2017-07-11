/*
 * File:   main.c
 * Author: Wouter
 */
#if defined(__XC16__)
#include <xc.h>
#elif defined(__C30__)
#if defined(__dsPIC33E__)
#include <p33Exxxx.h>
#elif defined(__dsPIC33F__)
#include <p33Fxxxx.h>
#endif
#endif
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <string.h>

#include "Settings.h"

#include "Drivers/INTERRUPT_Driver.h"
#include "Drivers/SYSTEM_Driver.h"
#include "Drivers/LCD_Driver.h"

#include "Controllers/LCD_Controller.h"

#include "../Common/Drivers/I2C_Driver.h"
#include "../Common/I2C_Settings.h"
#include "../Common/COM_Settings.h"
#include "../Common/MENU_Settings.h"




/*******************************************************************************
 *          DEFINES
 ******************************************************************************/

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
typedef struct {
    uint8_t command;
    int16_t extra;
} action_t;

typedef struct {
    action_t actions[8];
    uint8_t actionCnt;
} actionBuffer_t;

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static i2cData_t readData;
static i2cAnswer_t answerData;

static bool receivingLcdCommands;
static bool executingLcdCommands;

static actionBuffer_t actionBuffer;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize();
static void fillLcdActionBuffer(i2cData_t * data);
static void executeLcdAction(uint8_t command, int16_t extra);

void initialize() {
    D_INT_EnableInterrupts(false);

    // Initialize system
    D_SYS_InitPll();
    D_SYS_InitOscillator();
    D_SYS_InitPorts();

    // Interrupts
    D_INT_Init();
    D_INT_EnableInterrupts(true);

    // I2C
    D_I2C_InitSlave(I2C_LCD_ADDRESS, &readData, &answerData);
}

void fillLcdActionBuffer(i2cData_t * data) {
    actionBuffer.actions[actionBuffer.actionCnt].command = data->command;
    actionBuffer.actions[actionBuffer.actionCnt].extra = ((data->data1 << 8) & 0xFF00) | (data->data2);
    
    actionBuffer.actionCnt++;
    
    if (actionBuffer.actionCnt >= 8) {
        // Errors........
    }
}

void executeLcdAction(uint8_t command, int16_t extra) {
    
    uint8_t com = (command >> 6) & 0x03;

    switch (com) {
        case COM_LCD_COM_DRAW:
        {
            uint8_t menuId = ((command >> 3) & 0x07);
            uint8_t subMenuId = command & 0x07;
            C_LCD_DrawMenu(menuId);
            C_LCD_DrawSubMenu(subMenuId);
        }
            break;

        case COM_LCD_COM_SELECT:
        {
            uint8_t what = (command >> 5) & 0x01;
            uint8_t select = (command >> 4) & 0x01;
            uint8_t id = command & 0x0F;
            C_LCD_SetSelected(what, id, select);
        }
            break;

        case COM_LCD_COM_SET:
        {
//            if (I2C_SlaveReadResult == I2C_MWRITE) {
                uint8_t id = command & 0x3F;
                C_LCD_SetFieldValue(id, extra);
//            }
        }
            break;
        default:
            break;
    }
}

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/

/*******************************************************************************
 *          MAIN PROGRAM
 ******************************************************************************/

int main(void) {

    initialize();

    D_I2C_Enable(true);

    while (1) {
        if (I2C_ReadyToRead) {

            I2C_ReadyToRead = false;
            if (I2C_SlaveReadResult >= I2C_OK) {
                
                uint8_t command = (readData.command >> 5) & 0x07;
                uint8_t commandData = (readData.command) & 0x1F;
                
                if (receivingLcdCommands) {
                    if (readData.command != 0b10101011) {
                        fillLcdActionBuffer(&readData);
                    } 
                }

                switch (command) {
                    case COM_INIT:
                        C_LCD_Init();
                        receivingLcdCommands = false;
                        executingLcdCommands = false;
                        actionBuffer.actionCnt = 0;
                        readData.status |= STA_INITIALIZED;
                        break;

                    case COM_ENABLE:
                        // Set enabled
                        readData.status |= STA_ENABLED;
                        break;

                    case COM_LCD:
                        if (commandData == COM_LCD_START) {
                            receivingLcdCommands = true;
                        }
                        if (commandData == COM_LCD_STOP) {
                            receivingLcdCommands = false;
                            executingLcdCommands = true;
                        }
                        break;

                    default:
                        break;
                }


            } else {
                D_LCD_ClearSreen();
                D_LCD_WriteString("I2C Error ");
                D_LCD_Goto(1, 0);
                D_LCD_WriteInt(I2C1STAT);
                D_I2C_Reset();
            }

        }
        
        // Execute commands
        if (executingLcdCommands) {
                     
            if (actionBuffer.actionCnt == 0) {
                executingLcdCommands = false;
            } else {
                // Command
                uint8_t command = actionBuffer.actions[0].command;
                int16_t extra = actionBuffer.actions[0].extra;
                executeLcdAction(command, extra);

                // Shift
                uint16_t i;
                for (i = 0; i < 7; i++) {
                    actionBuffer.actions[i] = actionBuffer.actions[i + 1];
                }
                actionBuffer.actionCnt--;
            }
        }
    }
    return 0;
}

//while (1) {
//        if (I2C_ReadyToRead) {
//            
//            I2C_ReadyToRead = false;
//            if (I2C_SlaveReadResult >= I2C_OK) {
//                
//                uint8_t command = readData.command;
//                uint8_t com = (command >> 6) & 0x03;
//                
//                switch (com) {
//                    case COM_LCD_COM_DRAW: {
//                        uint8_t menuId = ((command >> 3) & 0x07);
//                        uint8_t subMenuId = command & 0x07;
//                        C_LCD_DrawMenu(menuId);
//                        C_LCD_DrawSubMenu(subMenuId);
//                    } break;
//                        
//                    case COM_LCD_COM_SELECT: {
//                        uint8_t what = (command >> 5) & 0x01;
//                        uint8_t select = (command >> 4) & 0x01;
//                        uint8_t id = command & 0x0F;
//                        C_LCD_SetSelected(what, id, select);
//                    } break;
//                        
//                    case COM_LCD_COM_SET: {
//                        if (I2C_SlaveReadResult == I2C_MWRITE) {
//                            uint8_t id = command & 0x3F;
//                            int16_t val = (readData.data2 << 8) | (readData.data1);
//                            C_LCD_SetFieldValue(id, val);
//                        }
//                    } break;
//                    default:
//                        break;
//                }
//
//            } else {
//                D_LCD_ClearSreen();
//                D_LCD_WriteString("I2C Error ");
//                D_LCD_Goto(1, 0);
//                D_LCD_WriteInt(I2C1STAT);
//                D_I2C_Reset();
//            }
//
//        }
//    }
//    return 0;


