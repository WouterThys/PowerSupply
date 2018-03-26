#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <string.h>
#include <math.h>

#include "utils.h"
#include "Settings.h"
#include "Drivers/SYSTEM_Driver.h"
#include "../Common/COM_Settings.h"

#include "Controllers/MENU_Controller.h"
#include "Controllers/UART_Controller.h"
#include "Controllers/SUPPLIES_Controller.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
#define VOLTAGE_STEP        100
#define COMMAND_BUFFER      5

typedef enum {
    S0,
    S1,
    S2,
    S3,
    S4
} FSMState_e;

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static volatile bool tmrFlag = false;
static FSMState_e fsmCurrentState = S0;
static FSMState_e fsmNextState = S0;

static SupplyData_t supVarData;
static SupplyData_t sup5V0Data;
static SupplyData_t sup3V3Data;

static LCD_Settings_t lcdSettings;

static volatile int16_t commandCnt = -1;
static volatile Command_t commandBuffer[COMMAND_BUFFER];

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize();
static void timerEnable(bool enable);

static bool putCommand(Command_t command);
static bool getCommand(Command_t * command);
static bool executeCommand(Command_t command);

void initialize() {
    sysInterruptEnable(false);

    // Initialize system
    sysInitPll();
    sysInitOscillator();
    sysInitPorts();

    // Interrupts
    sysInitInterrupts();
    sysInterruptEnable(true);
}

void timerEnable(bool enable) {
    T4CONbits.TON = 0; // Disable
    T4CONbits.TCS = 0; // Internal clock (Fp)
    T4CONbits.T32 = 0; // 16-bit timer
    T4CONbits.TCKPS = 0b11; // 1:256
    
    // Registers
    TMR4 = 0x0000;
    PR4 = 1000;
    
    // Interrupts
    _T4IP = IP_MAIN_TMR; 
    _T4IF = 0; // Clear
    _T4IE = 1; // Enable
    
    if (enable) {
        T4CONbits.TON = 1; // Enable
    } 
}

bool putCommand(Command_t command) {
    
    if (commandCnt >= COMMAND_BUFFER-1) {
        LED1 = 1;
        return false;
    }
    
    LED1 = 0;
    
    // Check if command already in buffer, overwrite data
    int16_t i;
    for (i = 0; i < commandCnt; i++) {
        if (command.command == commandBuffer[i].command) {
            commandBuffer[i].data = command.data;
            return true;
        }
    }
    
    // Put new command in buffer
    commandCnt++;
    commandBuffer[commandCnt] = command;
    return true;
}

bool getCommand(Command_t * command) {
    if (commandCnt < 0) {
        return false;
    }
    
    *command = commandBuffer[0]; // Always take first element
    
    // Shift
    uint16_t i;
    for (i = 1; i <= commandCnt; i++) {
        commandBuffer[i-1] = commandBuffer[i];
    }
    commandCnt--;
    
    return true;
}

bool executeCommand(Command_t command) {
    switch (command.command) {
        // Supply stuff
        case C_SET_VOLTAGE:
            supVarData.changed = true;
            supVarData.setVoltage = command.data;
            break;
        case C_SET_CURRENT:
            supVarData.changed = true;
            supVarData.setCurrent = command.data;
            break;

        // LCD stuff
        case C_LCD_CONTRAST:
            lcdSetDisplayContrast(command.data); // 1 - 50
            break;
        case C_LCD_BRIGHTNESS:
            lcdSetDisplayBrightness(command.data); // 1 - 8
            break;
    }
    return true;
}

/*******************************************************************************
 *          MAIN PROGRAM
 ******************************************************************************/
int main(void) {

    initialize();
    DelayMs(100);
    
    suppliesInit();
    uartInit(&putCommand);
    menuInit(&putCommand);
    DelayMs(100);
    
    lcdSettings.on = 1;
    lcdSettings.contrast = 40;
    lcdSettings.brightness = 6;
    timerEnable(true);
    
    while (1) {
        
        if (tmrFlag) {
            
            // Determine next state
            switch(fsmCurrentState) {
                default:
                case S0: fsmNextState = S1;
                    break;
                case S1: fsmNextState = S2;
                    break;
                case S2: fsmNextState = S3;
                    break;
                case S3: fsmNextState = S4;
                    break;
                case S4: fsmNextState = S0;
                    break;
            }
            
            Command_t command;
            
            // Handle current state
            switch(fsmCurrentState) {
                default:
                case S0: 
                    // Get commands from UART or MENU
                    while (getCommand(&command)) {
                        executeCommand(command);
                    }
                    break;
                case S1: 
                    // Get and set data from variable
                    if (supVarData.changed) {
                        setVoltage(supVarData.setVoltage);
                    }
                    getVarData(&supVarData);
                    break;
                case S2: 
                    // Get data from 5V
                    break;
                case S3: 
                    // Get data from 3.3V
                    break;
                case S4: 
                    // Update menu
                    menuUpdate(
                            lcdSettings,
                            supVarData,
                            sup5V0Data,
                            sup3V3Data);
                    break;
            }
            
            fsmCurrentState = fsmNextState;
            
            tmrFlag = false;
        }
        
    }
    return 0;
}

void __attribute__ ( (interrupt, no_auto_psv) ) _T4Interrupt(void) {
    if (_T4IF) {
        tmrFlag = true;
        _T4IF = 0; // Clear interrupt
    }
}

// UART
//        if (uartGetData(&uartData)) {
//            uint16_t value;
//            switch (uartData.command) {
//                // Basic
//                case C_HANDSHAKE:
//                    break;
//                    
//                // Supply stuff
//                case C_SET_VOLTAGE:
//                    concatinate(uartData.message[0], uartData.message[1], &value);
//                    setVoltage(value);
//                    menuSetVariableVoltage(value);
//                    break;
//                case C_SET_CURRENT:
//                    concatinate(uartData.message[0], uartData.message[1], &value);
//                    setCurrent(value);
//                    menuSetVariableCurrent(value);
//                    break;
//                case C_GET_VOLTAGE:
//                    
//                    break;
//                case C_GET_CURRENT:
//                    
//                    break;
//                    
//                // LCD stuff
//                case C_LCD_CONTRAST:
//                    //lcdSetDisplayContrast(uartData.message[0]); // 1 - 50
//                    break;
//                case C_LCD_BRIGHTNESS:
//                    //lcdSetDisplayBrightness(uartData.message[0]); // 1 - 8
//                    break;
//
//                // Voltage & current stuff
//            }
//        }

