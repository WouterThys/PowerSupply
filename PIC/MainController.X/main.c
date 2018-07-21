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
 * 
 * 
 * Time measurements
 * -----------------
 * 
 * -> FSM: 
 *      - heartbeat: 1kHz
 *      - 
 * 
 * -> I2C: clk = 200kHz
 *      - set voltage: 200µs <-> 250µs
 *      - set current: 200µs <-> 250µs
 *      - get measurement: 600µs <-> 800µs
 * 
 * -> MCP
 *      - get port: 18µs <-> 20µs
 * 
 * -> MENU
 *      - draw measurement 1: 20ms <-> 25ms
 *      - draw measurement 2: 6ms <-> 8ms
 * 
 *      - draw set voltage 1:
 *      - draw set voltage 2:
 * 
 *      - draw ...
 * 
 * */

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
#define VOLTAGE_STEP        40                      /* Step size in [mV]      */
#define VOLTAGE_MAX         4000                    /* Maximum in [mV]        */
#define VOLTAGE_MIN         40                      /* Minimum in [mV]        */
#define CURRENT_STEP        40                      /* Step size in [mA]      */
#define CURRENT_MAX         2000                    /* Maximum in [mA]        */
#define CURRENT_MIN         0                       /* Minimum in [mA]        */

#define FSM_PRE_SCALE       200     /* Timer with 1ms -> 200ms                */
#define FSM_MAX_WAIT_CNT    15      /* FSM at 200ms -> wait of 3s             */ 

#define COMMAND_BUFFER      5

typedef enum {
    S_INIT, // Initialize the LCD
    S_SHOW_MEASURE, // Show measured data from I²C
    S_SEL_VOLTAGE,  // Select the voltage
    S_CHA_VOLTAGE,  // Change the voltage
    S_SEL_CURRENT,  // Select the current
    S_CHA_CURRENT   // Change the current
} FSMState_e;

typedef struct {
    bool execute;            // Flag to indicate the FSM should execute
    FSMState_e currentState; // Current state of the FSM
    FSMState_e nextState;    // Next calculated state
    uint16_t waitCnt;        // Delay count
    uint16_t prescale;       // Pre-scale counter
} FSM_t;


/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static volatile FSM_t mainFsm;
static volatile Button_e prevBtnState = Open;
static volatile Button_e tmpBtnState = Open;
static volatile int16_t prevTurns = 0;
static volatile bool updateMsrData = false;
static volatile bool updateMenu = true;

static SupplyData_t supplyData;
static LCD_Settings_t lcdSettings;

static volatile int16_t encTurns = 0;
static volatile Button_e encButtonState;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize();
static void timerEnable(bool enable);

static bool putCommand(Command_t command);

static void heartBeat();
static void fsmCheckInputs();
static void fsmCalculateNextState(volatile FSM_t * fsm, int16_t turns, Button_e buttonState);
static void fsmHandeState(volatile FSM_t * fsm, int16_t turns);

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
    return true;
}

void heartBeat() {
    
    // Check user inputs
    fsmCheckInputs();
    
    // Update measured data
    if (mainFsm.prescale == 0) {
        updateMsrData = true;
    }
    
    // Check work
    mainFsm.prescale++;
    
    // FSM
    if (mainFsm.prescale > FSM_PRE_SCALE) {
        mainFsm.prescale = 0;

        // Update turns
        encTurns = prevTurns;
        prevTurns = 0;

        // Update button
        encButtonState = tmpBtnState;
        tmpBtnState = Open;
        prevBtnState = Open;

        // Flag
        mainFsm.execute = true;
        
    }
    
}

void fsmCheckInputs() {
    // Update data
    encDriverService();
    
    // Turns -> increment
    prevTurns += encDriverGetValue();

    // Button state -> update if needed
    Button_e newBtnState = encDriverGetButton();
    if (newBtnState > prevBtnState) {
        tmpBtnState = newBtnState;
    }

    prevBtnState = newBtnState;
}

void fsmCalculateNextState(volatile FSM_t * fsm, int16_t turns, Button_e buttonState) {
    
    fsm->nextState = fsm->currentState;
    
    // Find next state
    switch (fsm->currentState) {
        case S_INIT: // 0
            fsm->nextState = S_SHOW_MEASURE;
            updateMenu = true;
            break;
            
        case S_SHOW_MEASURE: // 1 
            // Stay in this state until user input change
            if ((turns != 0) || (buttonState != Open)) {
                fsm->nextState = S_SEL_VOLTAGE;
            }
            break;
            
        case S_SEL_VOLTAGE: // 2
            // Check if turn or click
            if (turns != 0) {
                turns = 0;
                fsm->nextState = S_SEL_CURRENT;
                fsm->waitCnt = 0;
            } else if (buttonState == Clicked) {
                updateMenu = true;
                fsm->nextState = S_CHA_VOLTAGE;
                fsm->waitCnt = 0;
            } else {
                fsm->waitCnt++;
                if (fsm->waitCnt >= FSM_MAX_WAIT_CNT) {
                    fsm->waitCnt = 0;
                    fsm->nextState = S_SHOW_MEASURE;
                    updateMenu = true;
                }
            }
            break;
            
        case S_CHA_VOLTAGE: // 3
            // Go back when clicked
            if (buttonState == Clicked) {
                fsm->nextState = S_SEL_VOLTAGE;
            }
            break;
            
        case S_SEL_CURRENT: 
            // Check if turn or click
            if (turns != 0) {
                turns = 0;
                fsm->nextState = S_SEL_VOLTAGE;
                fsm->waitCnt = 0;
            } else if (buttonState == Clicked) {
                updateMenu = true;
                fsm->nextState = S_CHA_CURRENT;
                fsm->waitCnt = 0;
            } else {
                fsm->waitCnt++;
                if (fsm->waitCnt >= FSM_MAX_WAIT_CNT) {
                    fsm->waitCnt = 0;
                    fsm->nextState = S_SHOW_MEASURE;
                    updateMenu = true;
                }
            }
            break;
            
        case S_CHA_CURRENT: 
             // Go back when clicked
            if (buttonState == Clicked) {
                fsm->nextState = S_SEL_VOLTAGE;
            }
            break;
            
        default:
            break;
    }
}

void fsmHandeState(volatile FSM_t * fsm, int16_t turns) {
    
    splUpdateData(&supplyData);
    
    switch (fsm->currentState) {
        case S_INIT: 
            // Initialize LCD 
            lcdInit();
            lcdTurnDisplayOn(lcdSettings.on);
            lcdSetDisplayBrightness(lcdSettings.brightness);
            lcdSetDisplayContrast(lcdSettings.contrast);
            
            break;
            
        case S_SHOW_MEASURE: 
            if (updateMenu ||
                    supplyData.msrVoltage.changed ||
                    supplyData.msrCurrent.changed ||
                    supplyData.msrTemperature.changed) {
                
                updateMenu = false;
                
                menuUpdateMeasuredData(
                        supplyData.msrVoltage.value,
                        supplyData.msrCurrent.value,
                        supplyData.msrTemperature.value
                        );
                
                supplyData.msrVoltage.changed = false;
                supplyData.msrCurrent.changed = false;
                supplyData.msrTemperature.changed = false;
            }
            break;
            
        case S_SEL_VOLTAGE: 
            menuSelectVoltage(supplyData.setVoltage.value);
            break;
            
        case S_CHA_VOLTAGE: 
            while (turns != 0) {
                if (turns > 0) {
                    if ((supplyData.setVoltage.value + VOLTAGE_STEP) < VOLTAGE_MAX) {
                        supplyData.setVoltage.value += VOLTAGE_STEP;
                    } else {
                        supplyData.setVoltage.value = VOLTAGE_MAX;
                    }
                    turns--;
                } else {
                    if ((supplyData.setVoltage.value - VOLTAGE_STEP) > VOLTAGE_MIN) {
                        supplyData.setVoltage.value -= VOLTAGE_STEP;
                    } else {
                        supplyData.setVoltage.value = VOLTAGE_MIN;
                    }
                    turns++;
                }
                supplyData.setVoltage.changed = true;
            }
            if (updateMenu || supplyData.setVoltage.changed) { 
                // Update LCD and Supplies (I²C)
                menuChangeVoltage(supplyData.setVoltage.value);
                splSetVoltage(supplyData.setVoltage.value);
                
                supplyData.setVoltage.changed = false;
                updateMenu = false;
            }
            break;
            
        case S_SEL_CURRENT: 
            menuSelectCurrent(supplyData.setCurrent.value);
            break;
            
        case S_CHA_CURRENT: 
            while (turns != 0) {
                if (turns > 0) {
                    if ((supplyData.setCurrent.value + CURRENT_STEP) < CURRENT_MAX) {
                        supplyData.setCurrent.value += CURRENT_STEP;
                    } else {
                        supplyData.setCurrent.value = CURRENT_MAX;
                    }
                    turns--;
                } else {
                    if ((supplyData.setCurrent.value - CURRENT_STEP) > CURRENT_MIN) {
                        supplyData.setCurrent.value -= CURRENT_STEP;
                    } else {
                        supplyData.setCurrent.value = CURRENT_MIN;
                    }
                    turns++;
                }
                supplyData.setCurrent.changed = true;
            }
            if (updateMenu || supplyData.setCurrent.changed) {
                // Update LCD and Supplies (I²C)
                menuChangeCurrent(supplyData.setCurrent.value);
                splSetCurrent(supplyData.setCurrent.value);
                
                supplyData.setCurrent.changed = false;
                updateMenu = false;
            }
            break;
            
        default:
            break;
    }
    
}

/*******************************************************************************
 *          MAIN PROGRAM
 ******************************************************************************/
int main(void) {

    initialize();
    DelayMs(100);
    
    uartInit(&putCommand);
    encDriverInit();
    suppliesInit();
    menuInit(&putCommand);
    DelayMs(100);
    
    // Initial values
    lcdSettings.on = 1;
    lcdSettings.contrast = 30;
    lcdSettings.brightness = 7;
    
    mainFsm.execute = false;
    mainFsm.currentState = S_INIT;
    mainFsm.nextState = S_INIT;
    mainFsm.prescale = 0;
    mainFsm.waitCnt = 0;
    
    DelayMs(100);
    timerEnable(true);
    
    if (DEBUG)  {
        printf("Start\n");
        printf(" Voltage step = %d\n", VOLTAGE_STEP);
        printf(" Voltage min = %d\n", VOLTAGE_MIN);
    }
    
    while (1) {
        
        // Fetch measured data
        if (updateMsrData) {
            updateMsrData = false;
            splUpdateMeasuremnets();
        }
        
        // Execute FSM
        if (mainFsm.execute) {
            mainFsm.execute = false;
            
            if (DEBUG_FSM && (encTurns != 0 || encButtonState != Open)) {
                printf("F T%d B%d\n", encTurns, encButtonState);
            }
            
            fsmCalculateNextState(&mainFsm, encTurns, encButtonState);
            
            if (DEBUG_FSM && (mainFsm.currentState != mainFsm.nextState)) {
                printf("F S%d>S%d\n", mainFsm.currentState, mainFsm.nextState);
            }
            
            fsmHandeState(&mainFsm, encTurns);
            
            mainFsm.currentState = mainFsm.nextState;
            
        }
        
    }
    return 0;
}


/*******************************************************************************
 *          INTERRUPTS
 ******************************************************************************/
void __attribute__ ( (interrupt, no_auto_psv) ) _T4Interrupt(void) {
    if (_T4IF) {
        heartBeat();
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

