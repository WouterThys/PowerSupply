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

#include "Controllers/LCD_Controller.h"

#include "../Common/Drivers/I2C_Driver.h"
#include "../Common/I2C_Settings.h"
#include "Drivers/LCD_Driver.h"


/*******************************************************************************
 *          DEFINES
 ******************************************************************************/

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static i2cData_t readData;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize();

void initialize() {
    D_INT_EnableInterrupts(false);
    
    // Initialize system
    D_SYS_InitPll();
    D_SYS_InitOscillator();
    D_SYS_InitPorts();
    
    // Interrupts
    D_INT_Init();
    D_INT_EnableInterrupts(true);
    
    // LCD
    C_LCD_Init();
    
    
    // I2C
    D_I2C_InitSlave(I2C_LCD_ADDRESS, &readData);
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
//    D_LCD_ClearSreen();
//    D_LCD_Goto(0,0);
//    D_LCD_WriteString("Turn:  ");
//    
//    D_LCD_Goto(1,0);
//    D_LCD_WriteString("Press: ");

    while(1) {
        if (I2C_ReadyToRead) {
            I2C_ReadyToRead = false;
            if (I2C_SlaveReadResult >= I2C_OK) {
                
                PORTAbits.RA0 = !PORTAbits.RA0;
//                
//                D_LCD_Goto(0,7);
//                D_LCD_WriteInt(readData.data2);
//                D_LCD_Goto(1,7);
//                D_LCD_WriteInt(readData.data1);

//                if (readData.data1 % 2 == 0) {
                    C_LCD_NextSubMenu();
//                }
                
            } else {
                D_LCD_ClearSreen();
                D_LCD_WriteString("I2C Error ");
                D_LCD_Goto(1,0);
                D_LCD_WriteInt(I2C1STAT);
                D_I2C_Reset();
            } 
            
            
        }
    }
    return 0;
}


