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
#include "../Common/COM_Settings.h"
#include "../Common/I2C_Settings.h"
#include "../Common/Drivers/I2C_Driver.h"

#include "Drivers/INTERRUPT_Driver.h"
#include "Drivers/SYSTEM_Driver.h"

#include "Controllers/DAC_Controller.h"
#include "Controllers/MCP4131_Controller.h"
#include "Drivers/ADC_Driver.h"




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
static i2cAnswer_t answerData;

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
    
    // I2C
    D_I2C_InitSlave(I2C_VARIABLE_ADDRESS, &readData, &answerData);
}

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/

/*******************************************************************************
 *          MAIN PROGRAM
 ******************************************************************************/

// -- Test PIC
// -----------------------------------------------------------------------------
//    
//    
//    
//
// -- Test DAC
// -----------------------------------------------------------------------------
//    C_DAC_Init(); // DAC and ADC
//    C_DAC_Enable(true);
//    C_DAC_Write(2000);

int main(void) {

    initialize();
    
//    C_DAC_Init(); // DAC and ADC
//    C_DAC_Enable(true);
//    C_DAC_Write(2000);
    
    while(1) {
        
    }

//    D_I2C_Enable(true);
//    
//    while (1) {
//        if (I2C_ReadyToRead) {
//            
//            I2C_ReadyToRead = false;
//            if (I2C_SlaveReadResult >= I2C_OK) {
//                uint8_t command = (readData.command >> 5) & 0x07;
//                uint8_t commandData = (readData.command)  & 0x1F;
//                
//                switch (command) {
//                    case COM_INIT:
//                        C_MCP4131_Init(); // Digital potentiometer
//                        C_DAC_Init(); // DAC and ADC
//                        D_ADC_Init(&answerData);
//                        readData.status |= STA_INITIALIZED;
//                        break;
//                        
//                    case COM_ENABLE:
//                        C_MCP4131_Enable(commandData);
//                        C_DAC_Enable(commandData);
//                        readData.status |= STA_ENABLED;
//                        break;
//                        
//                    case COM_READ:
//                        D_ADC_Enable(true);
//                        C_DAC_Write(2000);
//                        break;
//                        
//                    case COM_ASK:
//                        /* ADC writes data in i2cAnswer_t buffer, I2C will 
//                         * immediately answer with the data in this buffer when
//                         * the "COM_ASK" command is detected.
//                         */
//                        if (ADC_flag) {
//                            ADC_flag = false;
//                        } else {
//                            // Error?
//                        }
//                        break;
//                        
//                    default:
//                        break;
//                }
//                
//
//            } else {
//                D_I2C_Reset();
//            }
//
//        }
//    }
//    return 0;
}


