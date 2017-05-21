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

#include "Controllers/COM_Driver.h"
#include "Controllers/LCD_Controller.h"

#include "../Common/MENU_Settings.h"


/*******************************************************************************
 *          DEFINES
 ******************************************************************************/

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/


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
    
    // LCD
    C_LCD_Init();
    
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
    
    C_LCD_Next();
    
//    // Go to 2nd field
//    C_COM_LcdSelect(TYPE_FIELD, false, ID_M0_SM0_F1);
//    DelayMs(2000);
//    // Go to arrow
//    C_COM_LcdSelect(TYPE_ARROW, false, 0);
//    DelayMs(2000);
//    // Select arrow
//    C_COM_LcdSelect(TYPE_ARROW, true, 0);
//    DelayMs(2000);
//    // Press arrow
//    C_COM_LcdDraw(ID_M0, ID_M0_SM1);
//    DelayMs(2000);
    

    while(1) {
        
//        if (ENC_Change) {
//            D_ENC_GetState(&encState);
//            C_COM_SendEncoderState(encState);
//        }
        
        DelayMs(20);
        
    }
    return 0;
}


