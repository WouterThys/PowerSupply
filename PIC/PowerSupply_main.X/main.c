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
#include "Drivers/ENC_Driver.h"
#include "Drivers/CLK_Driver.h"
#include "../Common/Drivers/I2C_Driver.h"

#include "Controllers/COM_Driver.h"
#include "Controllers/LCD_Controller.h"
#include "Drivers/CLK_Driver.h"


/*******************************************************************************
 *          DEFINES
 ******************************************************************************/

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static enc_t encState;
static cursor_t lcdCursor;

static uint16_t retryCnt = 0;

static int16_t ADC_Data[4];
static bool change_0 = false;
static bool change_1 = false;
static bool change_2 = false;
static bool change_3 = false;
/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize();

void initialize() {
    D_INT_EnableInterrupts(false);
    LED1 = 1;

    // Initialize system
    D_SYS_InitPll();
    D_SYS_InitOscillator();
    D_SYS_InitPorts();

    // Interrupts
    D_INT_Init();
    D_INT_EnableInterrupts(true);

    // Communication
    C_COM_Init();

    // Clock
    D_CLK_Init();

    // LCD
    C_LCD_Init(&lcdCursor);

    // Rotary encoder
    D_ENC_Init();
}

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/

/*******************************************************************************
 *          MAIN PROGRAM
 ******************************************************************************/

int main(void) {

    initialize();

    C_COM_Enable(true);
    D_ENC_Enable(true);
    D_CLK_Enable(true);

    while (1) {

        if (CLK_Flag) {
            CLK_Flag = false;
            LED1 = !LED1;
            int16_t i2cResult = -1;
            // Read from variable 
            LED2 = 1;
            i2cResult = C_COM_VarRead(0);
            if (i2cResult > I2C_NOK) {
                if (ADC_Data[0] != i2cResult) {
                    change_0 = true;
                    ADC_Data[0] = i2cResult;
                }
            }
            i2cResult = C_COM_VarRead(1);
            if (i2cResult > I2C_NOK) {
                if (ADC_Data[1] != i2cResult) {
                    change_1 = true;
                    ADC_Data[1] = i2cResult;
                }
            }
            i2cResult = C_COM_VarRead(2);
            if (i2cResult > I2C_NOK) {
                if (ADC_Data[2] != i2cResult) {
                    change_2 = true;
                    ADC_Data[2] = i2cResult;
                }
            }
            i2cResult = C_COM_VarRead(3);
            if (i2cResult > I2C_NOK) {
                if (ADC_Data[3] != i2cResult) {
                    change_3 = true;
                    ADC_Data[3] = i2cResult;
                }
            }
            LED2 = 0;
            
            if (change_0) {
                C_COM_LcdSet(ID_M0_SM1_F0, ADC_Data[0]);
                change_0 = false;
            }
            
            if (change_1) {
                C_COM_LcdSet(ID_M0_SM1_F1, ADC_Data[1]);
                change_1 = false;
            }
            
            // Check rotary encoder and buttons
            if (ENC_Change) {
                D_ENC_GetState(&encState);
                if (encState.press == PRESS) {
                    C_LCD_Press(&lcdCursor);
                }
                if (encState.turn != NONE) {
                    C_LCD_Turn(&lcdCursor, encState);
                }
            }
        }

        if (lcdCursor.nextActionCnt > 0) {
            int16_t i2cResult = -1;

            switch (lcdCursor.nextActions[0]) {
                case DRAW:
                    i2cResult = C_COM_LcdDraw(lcdCursor.currentMenu->id, lcdCursor.currentSubMenu->id);
                    break;
                case SELECT:
                    if (lcdCursor.currentType == ARROW) {
                        i2cResult = C_COM_LcdSelect(TYPE_ARROW, false, lcdCursor.currentId);
                    } else if (lcdCursor.currentType == FIELD) {
                        i2cResult = C_COM_LcdSelect(TYPE_FIELD, (lcdCursor.current.field->selected == SELECTED), lcdCursor.currentId);
                    }
                    break;
                case SET:
                    i2cResult = C_COM_LcdSet(lcdCursor.currentId, lcdCursor.current.field->value);
                    break;
                default:
                    break;
            }

            if (i2cResult >= I2C_OK) {
                // Shift actions
                uint8_t i;
                for (i = 0; i < 3; i++) {
                    lcdCursor.nextActions[i] = lcdCursor.nextActions[i + 1];
                }
                lcdCursor.nextActionCnt--;
            } else {
                C_COM_I2CReset();
                retryCnt++;
                //                if (retryCnt > 3) {
                //                    // Horror
                //                }
            }
        }


    }
    return 0;
}


