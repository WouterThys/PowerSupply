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
static i2cData_t testData;

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
    
    testData.address = 0x56;
    testData.command = 6;
    
    LED1 = 1;
    DelayMs(2000);
    
    if (D_I2C_Write(&testData) == I2C_OK) {
        LED1 = 1;
        
    } 
    DelayUs(100);
    testData.address = 0x55;
    if (D_I2C_Write(&testData) == I2C_OK) {
        LED1 = 1;
    } 
    while(1) {
        
    }
    return 0;
}


