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
 *          VARIABLES
 ******************************************************************************/
static i2cData_t readData;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize();

void initialize() {
    D_INT_EnableInterrupts(false);
    LED1 = 0;
    
    // Initialize system
    D_SYS_InitPll();
    D_SYS_InitOscillator();
    D_SYS_InitPorts();
    
    // Interrupts
    D_INT_Init();
    D_INT_EnableInterrupts(true);
    
    // I2C
    D_I2C_InitSlave(I2C_UART_ADDRESS);
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
    LED1 = 1;
    readData.data1 = 0b00001111;
    readData.data2 = 0b11110000;
    readData.status = 3;
    
    while(1) {
        if (I2C_ReadyToRead) {
            LED1 = 0;
            if (D_I2C_SlaveRead(&readData) == I2C_OK) {
                LED1 = 1;
            } else {
                LED1 = 1;
                D_I2C_Reset();
                DelayUs(10);
                LED1 = 0;
                
            }
        }
    }
    return 0;
}


