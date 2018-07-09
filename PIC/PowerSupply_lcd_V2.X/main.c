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
#include "Drivers/ENC_Driver.h"
#include "../Common/COM_Settings.h"

#include "Controllers/MENU_Controller.h"
#include "Controllers/UART_Controller.h"
#include "Controllers/SUPPLIES_Controller.h"

/*******************************************************************************
 *          TODO
 *******************************************************************************
 * - Timer which will update the measured data every 1/10(?) sec with one
 *   I²C call, that will get:
 *      - measured voltage
 *      - measured current
 *      - measured temperature
 *   and set the local measured values.
 * 
 * - Timer (T2) which will get the user input. Create FSM to handle user input
 *   and set the:
 *      - set voltage
 *      - set current
 * 
 * 
 * */

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
#define VOLTAGE_STEP        100     /* Step size of voltage in [mV]           */
#define VOLTAGE_MAX         10000   /* Maximum voltage in [mV]                */
#define VOLTAGE_MIN         100     /* Minimum voltage in [mV]                */
#define CURRENT_STEP        100     /* Step size of current in [mA]           */
#define CURRENT_MAX         2000    /* Maximum current in [mA]                */
#define CURRENT_MIN         100     /* Minimum current in [mA]                */

#define FSM_PRE_SCALE       200     /* Timer with 1ms -> 200ms                */
#define FSM_MAX_WAIT_CNT    25      /* FSM at 200ms -> wait of 5s             */ 

#define COMMAND_BUFFER      5

typedef enum {
    S_INIT, // Initialize the LCD
    S_SHOW_MEASURE, // Show measured data from I²C
    S_SEL_VOLTAGE,  // Select the voltage
    S_CHA_VOLTAGE,  // Change the voltage
    S_SEL_CURRENT,  // Select the current
    S_CHA_CURRENT   // Change the current
} FSMState_e;

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static volatile bool tmrFlag = false;
static FSMState_e fsmCurrentState = S_INIT;
static FSMState_e fsmNextState = S_INIT;
static uint16_t fsmWaitCnt = 0;
static uint16_t fsmPreScale = 0;

static SupplyData_t supVarData;

static LCD_Settings_t lcdSettings;

static volatile int16_t commandCnt = -1;
static volatile Command_t commandBuffer[COMMAND_BUFFER];

static volatile int16_t encTurns = 0;
static volatile Button_e encButtonState;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize();
static void timerEnable(bool enable);

static bool putCommand(Command_t command);
static bool getCommand(Command_t * command);
static bool executeCommand(Command_t command);

static void clearChange(SupplyData_t * supplyData);

static void fsmCalculateNextState(FSMState_e currentState, FSMState_e * nextState, int16_t turns, Button_e buttonState);
static void fsmHandeState(FSMState_e currentState, int16_t turns);

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
    T4CONbits.TCKPS = 0b11; // 1:256 -> 143.948,3 Hz (6.95 µs)
    
    // Registers
    TMR4 = 0x0000;
    if (DEBUG) {
        PR4 = 70;
    } else {
        PR4 = 70; // 6.95µs * 144 = 1ms
    }
    
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
        return false;
    }
    
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
            supVarData.setVoltage.value = command.data;
            supVarData.setVoltage.changed = true;
            if (DEBUG) printf("Execute command: C_SET_VOLTAGE\n");
            break;
        case C_SET_CURRENT:
            supVarData.setCurrent.value = command.data;
            supVarData.setCurrent.changed = true;
            if (DEBUG) printf("Execute command: C_SET_CURRENT\n");
            break;

        // LCD stuff
        case C_LCD_CONTRAST:
            lcdSetDisplayContrast(command.data); // 1 - 50
            if (DEBUG) printf("Execute command: C_LCD_CONTRAST\n");
            break;
        case C_LCD_BRIGHTNESS:
            lcdSetDisplayBrightness(command.data); // 1 - 8
            if (DEBUG) printf("Execute command: C_LCD_BRIGHTNESS\n");
            break;
    }
    return true;
}

void clearChange(SupplyData_t * supplyData)  {
    supplyData->setVoltage.changed = false;
    supplyData->setCurrent.changed = false;
    supplyData->msrVoltage.changed = false;
    supplyData->msrCurrent.changed = false;
    supplyData->msrTemperature.changed = false;
}

/*******************************************************************************
 *          MAIN PROGRAM
 ******************************************************************************/
int main(void) {

    initialize();
    DelayMs(100);
    
    suppliesInit();
    uartInit(&putCommand);
    encDriverInit();
    menuInit(&putCommand);
    DelayMs(100);
    
    lcdSettings.on = 1;
    lcdSettings.contrast = 30;
    lcdSettings.brightness = 7;
    
    supVarData.setVoltage.value = 1000;
    supVarData.setVoltage.changed = true;
    
    supVarData.setCurrent.value = 1000;
    supVarData.setCurrent.changed = true;
    
    DelayMs(100);
    timerEnable(true);
    
    if (DEBUG) printf("start\n");
    fsmHandeState(S_INIT, 0);
    
    while (1) {
        
        if (tmrFlag) {
            tmrFlag = false;
            fsmCalculateNextState(fsmCurrentState, &fsmNextState, encTurns, encButtonState);
            
            if (fsmCurrentState != fsmNextState) {
                if (DEBUG) printf("FSM S%d>S%d\n", fsmCurrentState, fsmNextState);
                fsmCurrentState = fsmNextState;
            }
            
            fsmHandeState(fsmCurrentState, encTurns);
        }
        
    }
    return 0;
}

void fsmCalculateNextState(FSMState_e currentState, FSMState_e * nextState, int16_t turns, Button_e buttonState) {
    
    *nextState = currentState;
    
    // Find next state
    switch (currentState) {
        case S_INIT: 
            *nextState = S_SHOW_MEASURE;
            break;
            
        case S_SHOW_MEASURE: 
            // Stay in this state until user input change
            if ((turns != 0) || (buttonState != Open)) {
                *nextState = S_SEL_VOLTAGE;
            }
            break;
            
        case S_SEL_VOLTAGE: 
            // Check if turn or click
            if (turns != 0) {
                turns = 0;
                *nextState = S_SEL_CURRENT;
                fsmWaitCnt = 0;
            } else if (buttonState == Clicked) {
                *nextState = S_CHA_VOLTAGE;
                fsmWaitCnt = 0;
            } else {
                fsmWaitCnt++;
                if (fsmWaitCnt >= FSM_MAX_WAIT_CNT) {
                    fsmWaitCnt = 0;
                    *nextState = S_SHOW_MEASURE;
                }
            }
            break;
            
        case S_CHA_VOLTAGE: 
            // Go back when clicked
            if (buttonState == Clicked) {
                *nextState = S_SEL_VOLTAGE;
            }
            break;
            
        case S_SEL_CURRENT: 
            // Check if turn or click
            if (turns != 0) {
                turns = 0;
                *nextState = S_SEL_VOLTAGE;
                fsmWaitCnt = 0;
            } else if (buttonState == Clicked) {
                *nextState = S_CHA_CURRENT;
                fsmWaitCnt = 0;
            } else {
                fsmWaitCnt++;
                if (fsmWaitCnt >= FSM_MAX_WAIT_CNT) {
                    fsmWaitCnt = 0;
                    *nextState = S_SHOW_MEASURE;
                }
            }
            break;
            
        case S_CHA_CURRENT: 
             // Go back when clicked
            if (buttonState == Clicked) {
                *nextState = S_SEL_VOLTAGE;
            }
            break;
            
        default:
            break;
    }
}

void fsmHandeState(FSMState_e currentState, int16_t turns) {
    
    switch (currentState) {
        case S_INIT: 
            // Initialize LCD 
            lcdInit();
            lcdTurnDisplayOn(lcdSettings.on);
            lcdSetDisplayBrightness(lcdSettings.brightness);
            lcdSetDisplayContrast(lcdSettings.contrast);
            break;
            
        case S_SHOW_MEASURE: 
            if (supVarData.msrVoltage.changed ||
                    supVarData.msrCurrent.changed ||
                    supVarData.msrTemperature.changed) {
                
                menuUpdateMeasuredData(
                        supVarData.msrVoltage.value,
                        supVarData.msrCurrent.value,
                        supVarData.msrTemperature.value
                        );
                
                supVarData.msrVoltage.changed = false;
                supVarData.msrCurrent.changed = false;
                supVarData.msrTemperature.changed = false;
            }
            break;
            
        case S_SEL_VOLTAGE: 
            menuSelectVoltage(supVarData.setVoltage.value);
            break;
            
        case S_CHA_VOLTAGE: 
            while (turns != 0) {
                if (turns > 0) {
                    if ((supVarData.setVoltage.value + VOLTAGE_STEP) < VOLTAGE_MAX) {
                        supVarData.setVoltage.value += VOLTAGE_STEP;
                    } else {
                        supVarData.setVoltage.value = VOLTAGE_MAX;
                    }
                    turns--;
                } else {
                    if ((supVarData.setVoltage.value - VOLTAGE_STEP) > VOLTAGE_MIN) {
                        supVarData.setVoltage.value -= VOLTAGE_STEP;
                    } else {
                        supVarData.setVoltage.value = VOLTAGE_MIN;
                    }
                    turns++;
                }
                supVarData.setVoltage.changed = true;
            }
            if (supVarData.setVoltage.changed) {
                // update supplies   
                if (DEBUG) printf("Vs=%dmV\n", supVarData.setVoltage.value);
                menuChangeVoltage(supVarData.setVoltage.value);
                supVarData.setVoltage.changed = false;
            }
            break;
            
        case S_SEL_CURRENT: 
            menuSelectCurrent(supVarData.setCurrent.value);
            break;
            
        case S_CHA_CURRENT: 
            while (turns != 0) {
                if (turns > 0) {
                    if ((supVarData.setCurrent.value + CURRENT_STEP) < CURRENT_MAX) {
                        supVarData.setCurrent.value += CURRENT_STEP;
                    } else {
                        supVarData.setCurrent.value = CURRENT_MAX;
                    }
                    turns--;
                } else {
                    if ((supVarData.setCurrent.value - CURRENT_STEP) > CURRENT_MIN) {
                        supVarData.setCurrent.value -= CURRENT_STEP;
                    } else {
                        supVarData.setCurrent.value = CURRENT_MIN;
                    }
                    turns++;
                }
                supVarData.setCurrent.changed = true;
            }
            if (supVarData.setCurrent.changed) {
                // update supplies   
                if (DEBUG) printf("Is=%dmA\n", supVarData.setVoltage.value);
                menuChangeCurrent(supVarData.setCurrent.value);
                supVarData.setCurrent.changed = false;
            }
            break;
            
        default:
            break;
    }
    
}

static volatile Button_e prevBtnState = Open;
static volatile int16_t turns = 0;
void __attribute__ ( (interrupt, no_auto_psv) ) _T4Interrupt(void) {
    if (_T4IF) {
        
        LED1 = !LED1;
        
        encDriverService(); // Update data
        
        // Turns -> increment
        turns += encDriverGetValue();
        
        // Button state -> update if needed
        Button_e newBtnState = encDriverGetButton();
        if (newBtnState > prevBtnState) {
            encButtonState = newBtnState;
        }
        
        prevBtnState = newBtnState;
        
        fsmPreScale++;
        if (fsmPreScale > FSM_PRE_SCALE) {
            fsmPreScale = 0;
            
            // Update turns
            encTurns = turns;
            turns = 0;
            
            // Update button
            prevBtnState = Open;
            
            // Flag
            tmrFlag = true;
        }
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

