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

#include "../Common/Drivers/I2C_Driver.h"
#include "../Common/I2C_Settings.h"


/*******************************************************************************
 *          DEFINES
 ******************************************************************************/

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static i2cData_t uartData;
static i2cData_t variableData;
static i2cData_t lcdData;
static enc_t encState;

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
    
    // I2C
    D_I2C_InitMaster();
    uartData.address = I2C_UART_ADDRESS;
    variableData.address = I2C_VARIABLE_ADDRESS;
    lcdData.address = I2C_LCD_ADDRESS;
    
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
    
    D_I2C_Enable(true);
    D_ENC_Enable(true);
    

    while(1) {
        

        if (ENC_Change) {
            D_ENC_GetState(&encState);
            if (encState.pressCount > 0 || encState.turnCount > 0) {
                lcdData.data1 = encState.pressCount;
                lcdData.data2 = encState.turnCount;
                D_I2C_MasterWrite(&lcdData);
            }
        }
        
        DelayMs(20);
        
    }
    return 0;
}


