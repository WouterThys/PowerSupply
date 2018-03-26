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
#include "Drivers/UART_Driver.h"
#include "Drivers/SPI_Driver.h"
#include "Drivers/ADC_Driver.h"
#include "Controllers/UART_Controller.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
#define COMMAND_VOLTAGE     "V"
/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
uint16_t ADCValues[4] = {0, 0, 0, 0};

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize();
static uint16_t stringToInt(READ_Data data);

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
    
    // Communication
    C_UART_Init();
    C_DAC_Init();
    
    // ADC
    D_ADC_Init();
    
}

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/

/*******************************************************************************
 *          MAIN PROGRAM
 ******************************************************************************/

int main(void) {
    
    initialize();
    
    D_ADC_Enable(true);
    C_DAC_Enable(true);

    
    while(1) {
        if (ADC_flag && !UART_flag) {
            
//            C_UART_WriteInt("I", ADC_Values[ADC_CURRENT]);
//            C_UART_WriteInt("T", ADC_Values[ADC_TEMPERATURE]);
            
            ADC_flag = false;
        }
        
        if (UART_flag) {
           
            READ_Data data = C_UART_Read();
            
            // TODO: to separate method
            //if (stringEquals(data.command, COMMAND_VOLTAGE)) {
            if (strcmp(data.command, COMMAND_VOLTAGE) == 0) {
                 LED1 = !LED1;
                 uint16_t val = stringToInt(data);
                C_DAC_Write(val);
                C_UART_WriteInt("V", val);
            }
            
            UART_flag = false;
        }
    }
    return 0;
}

uint16_t stringToInt(READ_Data data) {
    
    uint16_t val = 0;
    uint16_t tmp = 0;
    uint16_t shift = 0;
    uint16_t index = 0;
    
    while(data.message[index] != '\0') {
        tmp = data.message[index] - 0x30;
        if (shift != 0) {
            val *= 10;
        }
        val += tmp;
        shift++;
        index++;
    }
    return val;
}

//uint16_t stringToInt(const char * str) {
//    uint16_t val = 0;
//    uint16_t tmp = 0;
//    uint16_t shift = 0;
//    while(*str != '\0') {
//        tmp = *str - 0x30;
//        if (shift != 0) {
//            val *= 10;
//        }
//        val += tmp;
//        shift++;
//        str++;
//    }
//    return val;
//}


