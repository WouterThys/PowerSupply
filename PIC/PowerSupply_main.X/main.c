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

#include "../Common/Drivers/I2C_Driver.h"
#include "../Common/I2C_Settings.h"


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
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize();
static i2cData_t uartData;
static i2cData_t variableData;
static i2cData_t lcdData;

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
    
    uartData.command = 1;
    variableData.command = 2;
    lcdData.command = 3;
    
    LED1 = 1;
    DelayMs(2000);
    LED1 = 0;
//    if (D_I2C_MasterRead(&variableData) == I2C_OK) {
//        LED1 = 1;
//        
//    } 
//    DelayUs(100);
//    if (D_I2C_MasterRead(&uartData) == I2C_OK) {
//        LED1 = 1;
//    } 
//    DelayUs(100);
    if (D_I2C_MasterRead(&lcdData) == I2C_OK) {
        LED1 = 1;
    } 
    while(1) {
        
    }
    return 0;
}


