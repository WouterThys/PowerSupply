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
#include "Drivers/I2C_Driver.h"
#include "../Common/COM_Settings.h"

#include "Controllers/MENU_Controller.h"
#include "Controllers/UART_Controller.h"
#include "Controllers/SUPPLIES_Controller.h"
#include "Controllers/GLCD_Controller.h"

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

#define FSM_PRE_SCALE       200     /* Timer with 1ms -> 200ms                */
#define FSM_MAX_WAIT_CNT    15      /* FSM at 200ms -> wait of 3s             */ 

#define COMMAND_BUFFER      5


/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static volatile MainFSM_t mainFsm;
static volatile CalibrationFSM_t calibrateFsm;
static volatile SettingsFSM_t settingsFsm;
static volatile ErrorFSM_t errorFsm;

static volatile Button_e prevBtnState = Open;
static volatile Button_e tmpBtnState = Open;
static volatile int16_t prevTurns = 0;
static volatile bool updateMsrData = false;
static volatile bool updateMenu = true;

static SupplyData_t supplyData;
static SupplyStatus_t supplyStatus; 
static uint16_t previousStatus = 0;

static volatile int16_t encTurns = 0;
static volatile Button_e encButtonState;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize();
static void timerEnable(bool enable);
static void heartBeat();
static void printStatus(SupplyStatus_t status);
static void printI2CError(int16_t errorCode);

static void fsmCheckInputs();
// Main FSM
static void mainFsmCalculateNextState(volatile MainFSM_t * fsm, int16_t turns, Button_e buttonState);
static void mainFsmHandeState(volatile MainFSM_t * fsm, volatile SettingsFSM_t * lcdSettings, int16_t turns);
// Calibration FSM
static void calibrateFsmCalcultateNextState(volatile CalibrationFSM_t * fsm, int16_t turns, Button_e buttonState);
static void calibrateFsmHandleState(volatile CalibrationFSM_t * fsm, int16_t turns);
// Settings FSM
static void settingsFsmCalculateNextState(volatile SettingsFSM_t * fsm, int16_t turns, Button_e buttonState);
static void settingsFsmHandleState(volatile SettingsFSM_t * fsm, int16_t turns);
// Error FSM
static void errorFsmCalculateNextState(volatile ErrorFSM_t * fsm, int16_t turns, Button_e buttonState);
static void errorFsmHandleState(volatile ErrorFSM_t * fsm);

static bool putCommand(Command_t command);
static void onError(Error_t error);
static void onSysError(int16_t error);

void initialize() {
    // Interrupts
    sysInitInterrupts();
    sysInterruptEnable(true);
    sysInitError(&onSysError);
    
    // Initialize system
    sysInitPll();
    sysInitOscillator();
    sysInitPorts();

    
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

void printStatus(SupplyStatus_t status) {
    printf("STATUS: \n");
    printf(" - Code: %d\n", status.statusCode);
    printf(" - Error: %d\n", status.errorCode);
    printf(" - Clip: %d\n", status.currentClip);
    printf(" - Calib: %d\n", status.calibrationSt);
    printf(" - O EN: %d\n", status.outputEnabled);
    printf(" - C EN: %d\n", status.calibrateEnabled);
    printf(" - P EN: %d\n", status.pidEnabled);
}

void printI2CError(int16_t errorCode) {
    printf("I2C ERROR!\n");
    switch(errorCode) {
        default: 
            printf("I2C_OK\n"); break;
            break;
        case I2C_NOK: printf("I2C_NOK\n"); break;
        case I2C_OVERFLOW: printf("I2C_OVERFLOW\n"); break;
        case I2C_COLLISION: printf("I2C_COLLISION\n"); break;
        case I2C_NO_ADR_ACK: printf("I2C_NO_ADR_ACK\n"); break;
        case I2C_NO_DATA_ACK: printf("I2C_NO_DATA_ACK\n"); break;
        case I2C_UNEXPECTED_DATA: printf("I2C_UNEXPECTED_DATA\n"); break;
        case I2C_UNEXPECTED_ADR: printf("I2C_UNEXPECTED_ADR\n"); break;
        case I2C_STILL_BUSY: printf("I2C_STILL_BUSY\n"); break;
        case I2C_TIMEOUT: printf("I2C_TIMEOUT\n"); break;
    }
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

void mainFsmCalculateNextState(
        volatile MainFSM_t * fsm, 
        int16_t turns, 
        Button_e buttonState) {
    
    fsm->nextState = fsm->currentState;
    
    // Check errors
    if (errorFsm.lastError.hasError) {
        if (fsm->currentState != M_ERROR) {
            fsm->saveState = fsm->currentState;
        }
        fsm->currentState = M_ERROR;
    }
    
    // Find next state
    switch (fsm->currentState) {
        case M_INIT: // 0
            fsm->nextState = M_SHOW_MEASURE1;
            updateMenu = true;
            break;
            
        case M_SHOW_MEASURE1: 
            // Stay in this state until user input change
            if (buttonState != Open) {
                fsm->nextState = M_SEL_VOLTAGE;
            } else if (turns != 0) {
                updateMenu = true;
                fsm->nextState = M_SHOW_MEASURE2;
            }
            break;
            
        case M_SHOW_MEASURE2:  
            // Stay in this state until user input change
            if (buttonState != Open) {
                fsm->nextState = M_SEL_VOLTAGE;
            } else if (turns != 0) {
                updateMenu = true;
                fsm->nextState = M_SHOW_MEASURE1;
            }
            break;
            
        case M_SEL_VOLTAGE: 
            // Check if turn or click
            if (turns != 0) {
                if (turns > 0) {
                    fsm->nextState = M_SEL_CURRENT;
                } else {
                    fsm->nextState = M_SEL_SETTINGS;
                }
                fsm->waitCnt = 0;
                turns = 0;
            } else if (buttonState == Clicked) {
                updateMenu = true;
                fsm->nextState = M_CHA_VOLTAGE;
                fsm->waitCnt = 0;
            } else {
                fsm->waitCnt++;
                if (fsm->waitCnt >= FSM_MAX_WAIT_CNT) {
                    fsm->waitCnt = 0;
                    fsm->nextState = M_SHOW_MEASURE1;
                    updateMenu = true;
                }
            }
            break;
            
        case M_CHA_VOLTAGE: 
            // Go back when clicked
            if (buttonState == Clicked) {
                fsm->nextState = M_SEL_VOLTAGE;
            }
            break;
            
        case M_SEL_CURRENT: 
            // Check if turn or click
            if (turns != 0) {
                if (turns > 0) {
                    fsm->nextState = M_SEL_CALIBRATION;
                } else {
                    fsm->nextState = M_SEL_VOLTAGE;
                }
                fsm->waitCnt = 0;
                turns = 0;
            } else if (buttonState == Clicked) {
                updateMenu = true;
                fsm->nextState = M_CHA_CURRENT;
                fsm->waitCnt = 0;
            } else {
                fsm->waitCnt++;
                if (fsm->waitCnt >= FSM_MAX_WAIT_CNT) {
                    fsm->waitCnt = 0;
                    fsm->nextState = M_SHOW_MEASURE1;
                    updateMenu = true;
                }
            }
            break;
            
        case M_CHA_CURRENT: 
             // Go back when clicked
            if (buttonState == Clicked) {
                fsm->nextState = M_SEL_CURRENT;
            }
            break;
            
        case M_SEL_CALIBRATION:
             // Check if turn or click
            if (turns != 0) {
                if (turns > 0) {
                    fsm->nextState = M_SEL_SETTINGS;
                } else {
                    fsm->nextState = M_SEL_CURRENT;
                }
                fsm->waitCnt = 0;
                turns = 0;
            } else if (buttonState == Clicked) {
                updateMenu = true;
                fsm->nextState = M_CHA_CALIBRATION;
                fsm->waitCnt = 0;
            } else {
                fsm->waitCnt++;
                if (fsm->waitCnt >= FSM_MAX_WAIT_CNT) {
                    fsm->waitCnt = 0;
                    fsm->nextState = M_SHOW_MEASURE1;
                    updateMenu = true;
                }
            }
            break;
            
        case M_CHA_CALIBRATION:
            calibrateFsmCalcultateNextState(&calibrateFsm, encTurns, encButtonState);
            if (calibrateFsm.nextState == C_STOP) {
                calibrateFsm.nextState = C_INIT;
                calibrateFsm.currentState = C_INIT;
                
                fsm->nextState = M_SEL_CALIBRATION;
            }
            break;
            
        case M_SEL_SETTINGS:
             // Check if turn or click
            if (turns != 0) {
                if (turns > 0) {
                    fsm->nextState = M_SEL_VOLTAGE;
                } else {
                    fsm->nextState = M_SEL_CALIBRATION;
                }
                fsm->waitCnt = 0;
                turns = 0;
            } else if (buttonState == Clicked) {
                updateMenu = true;
                fsm->nextState = M_CHA_SETTINGS;
                fsm->waitCnt = 0;
            } else {
                fsm->waitCnt++;
                if (fsm->waitCnt >= FSM_MAX_WAIT_CNT) {
                    fsm->waitCnt = 0;
                    fsm->nextState = M_SHOW_MEASURE1;
                    updateMenu = true;
                }
            }
            break;
            
        case M_CHA_SETTINGS:
            settingsFsmCalculateNextState(&settingsFsm, encTurns, encButtonState);
            if (settingsFsm.nextState == S_STOP) {
                settingsFsm.nextState = S_INIT;
                settingsFsm.currentState = S_INIT;
                
                fsm->nextState = M_SEL_SETTINGS;
            }
            break;
            
            
        case M_ERROR:
            errorFsmCalculateNextState(&errorFsm, encTurns, encButtonState);
            if (errorFsm.nextState == E_STOP) {
                errorFsm.currentState = E_INIT;
                errorFsm.nextState = E_INIT;
                
                fsm->nextState = fsm->saveState;
                fsm->saveState = 0;
            }
            break;
            
        default:
            break;
    }
}

void mainFsmHandeState(
        volatile MainFSM_t * fsm, 
        volatile SettingsFSM_t * lcdSettings, 
        int16_t turns) {
    
    splUpdateData(&supplyData); // Get measurement data
    
    switch (fsm->currentState) {
        case M_INIT: 
            // Initialize LCD 
            lcdInit();
            lcdTurnDisplayOn(true);
            lcdSetDisplayBrightness(lcdSettings->brightness);
            lcdSetDisplayContrast(lcdSettings->contrast);
            
            // Default values
            supplyData.setVoltage.value = DEFAULT_V;
            supplyData.setCurrent.value = DEFAULT_I;
            splSetVoltage(supplyData.setVoltage.value);
            splSetCurrent(supplyData.setCurrent.value);
            break;
            
        case M_SHOW_MEASURE1: 
            if (updateMenu ||
                    supplyData.msrVoltage.changed ||
                    supplyData.msrCurrent.changed) {
                
                updateMenu = false;
                
                menuUpdateMeasured1Data(
                        supplyData.msrVoltage.value,
                        supplyData.msrCurrent.value,
                        supplyStatus.currentClip);
                
                supplyData.msrVoltage.changed = false;
                supplyData.msrCurrent.changed = false;
            }
            break;
            
        case M_SHOW_MEASURE2: 
            if (updateMenu || supplyData.msrTemperature.changed) {
                
                updateMenu = false;
                menuUpdateMeasured2Data(supplyData.msrTemperature.value, 0);
                supplyData.msrTemperature.changed = false;
            }
            break;
            
        case M_SEL_VOLTAGE: 
            menuSelectVoltage(supplyData.setVoltage.value);
            break;
            
        case M_CHA_VOLTAGE: 
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
            
        case M_SEL_CURRENT: 
            menuSelectCurrent(supplyData.setCurrent.value);
            break;
            
        case M_CHA_CURRENT: 
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
            
        case M_SEL_CALIBRATION:
            menuSelectCalibration();
            break;
            
        case M_CHA_CALIBRATION:
            calibrateFsmHandleState(&calibrateFsm, encTurns);
            break;
            
        case M_SEL_SETTINGS:
            menuSelectSettings();
            break;
            
        case M_CHA_SETTINGS:
            settingsFsmHandleState(&settingsFsm, encTurns);
            break;
            
        case M_ERROR:
            errorFsmHandleState(&errorFsm);
            break;
            
        default:
            break;
    }
    
}

void calibrateFsmCalcultateNextState(
        volatile CalibrationFSM_t * fsm, 
        int16_t turns, 
        Button_e buttonState) {
    
    fsm->nextState = fsm->currentState;
    
    // Find next state
    switch (fsm->currentState) {
        case C_INIT:
            fsm->saveState = C_INIT;
            fsm->nextState = C_SEND_TO_SLAVE;
            updateMenu = true;
            break;
            
        case C_SET_DESIRED:
            fsm->saveState = C_SET_DESIRED;
            fsm->nextState = C_SEND_TO_SLAVE;
            updateMenu = true;
            break;
            
        case C_CALIBRATE:
            if (buttonState == Clicked) {
                fsm->nextState = C_SAVE;
                updateMenu = true;
            }
            break;
            
        case C_SAVE:
            fsm->saveState = C_SAVE;
            fsm->nextState = C_SEND_TO_SLAVE;
            break;
            
        case C_DONE:
            fsm->saveState = C_DONE;
            fsm->nextState = C_SEND_TO_SLAVE;
            break;
            
            
        case C_SEND_TO_SLAVE:
            fsm->nextState = C_WAIT_FOR_SLAVE;
            break;
            
        case C_WAIT_FOR_SLAVE:
            if (supplyStatus.calibrationSt == fsm->saveState) {
                switch (fsm->saveState) {
                    case C_INIT: 
                        fsm->nextState = C_SET_DESIRED; 
                        break;
                    case C_SET_DESIRED: 
                        fsm->nextState = C_CALIBRATE; 
                        break;
                    case C_SAVE:
                        if (fsm->calibrationCount < 10) {
                            fsm->nextState = C_SET_DESIRED;
                        } else {
                            fsm->nextState = C_DONE;
                        }
                        break;
                    case C_DONE:
                        fsm->nextState = C_STOP;
                        break;
                    default:
                        break;
                }
            }
            break;
            
        default:
            break;
    }
}

void calibrateFsmHandleState(volatile CalibrationFSM_t * fsm, int16_t turns) {
    
    switch (fsm->currentState) {
        case C_INIT:
            fsm->calibrationCount = 0;
            fsm->desiredVoltage = 0;
            if (updateMenu) {
                menuChangeCalibration(fsm->desiredVoltage, fsm->calibratedVoltage);
                updateMenu = false;
            }
            break;
            
        case C_SET_DESIRED:
            if (fsm->desiredVoltage == 0) {
                fsm->desiredVoltage = CALIB_MIN;
            } else {
                fsm->desiredVoltage = fsm->desiredVoltage + CALIB_STEP;
            }
            fsm->calibratedVoltage = fsm->desiredVoltage;
            splSetVoltage(fsm->calibratedVoltage);
            if (updateMenu) {
                menuChangeCalibration(fsm->desiredVoltage, fsm->calibratedVoltage);
                updateMenu = false;
            }
            break;
            
        case C_CALIBRATE:
            while (turns != 0) {
                if (turns > 0) {
                    fsm->calibratedVoltage += CALIB_mSTEP;
                    turns--;
                } else {
                    fsm->calibratedVoltage -= CALIB_mSTEP;
                    turns++;
                }
                updateMenu = true;
            }
            if (updateMenu) { 
                // Update LCD and Supplies (I²C)
                menuChangeCalibration(fsm->desiredVoltage, fsm->calibratedVoltage);
                splSetVoltage(fsm->calibratedVoltage);
                updateMenu = false;
            }
            break;
            
        case C_SAVE: 
            fsm->calibrationCount += 1;
            break;
            
        case C_DONE:
            break;
            
            
         case C_SEND_TO_SLAVE:
            splSetCalibration(*fsm);
            break;
            
        case C_WAIT_FOR_SLAVE:
            break;
            
        default:
            break;
    }
}


void settingsFsmCalculateNextState(
        volatile SettingsFSM_t * fsm, 
        int16_t turns, 
        Button_e buttonState) {
    
    
    fsm->nextState = fsm->currentState;
    
    // Find next state
    switch (fsm->currentState) {
        case S_INIT:
            fsm->nextState = S_SEL_CONTRAST;
            updateMenu = true;
            break;
            
        case S_SEL_BRIGHTNESS:
            if (buttonState != Open) {
                if (buttonState == DoubleClicked) {
                    fsm->nextState = S_STOP;
                } else {
                    fsm->nextState = S_CHA_BRIGHTNESS;
                }
            } else if (turns != 0) {
                updateMenu = true;
                fsm->nextState = S_SEL_CONTRAST;
            }
            break;
            
        case S_CHA_BRIGHTNESS:
            if (buttonState == Clicked) {
                fsm->nextState = S_SEL_BRIGHTNESS;
            }
            break;
            
        case S_SEL_CONTRAST:
            if (buttonState != Open) {
                if (buttonState == DoubleClicked) {
                    fsm->nextState = S_STOP;
                } else {
                    fsm->nextState = S_CHA_CONTRAST;
                }
            } else if (turns != 0) {
                updateMenu = true;
                fsm->nextState = S_SEL_BRIGHTNESS;
            }
            break;
            
        case S_CHA_CONTRAST:
            if (buttonState == Clicked) {
                fsm->nextState = S_SEL_CONTRAST;
            }
            break;
            
        case S_STOP:
            fsm->nextState = S_INIT;
            break;
            
        default:
            break;
    }
    
}

void settingsFsmHandleState(volatile SettingsFSM_t * fsm, int16_t turns) {
    
    switch (fsm->currentState) {
        case S_INIT:
            break;
            
        case S_SEL_BRIGHTNESS:
            if (updateMenu) {
                menuSelectBrightness(fsm->brightness, fsm->contrast);
                updateMenu = false;
            }
            break;
            
        case S_CHA_BRIGHTNESS:
             while (turns != 0) {
                if (turns > 0) {
                    if (fsm->brightness < 8) {
                        fsm->brightness += 1;
                        updateMenu = true;
                    }
                    turns--;
                } else {
                    if (fsm->brightness > 1) {
                        fsm->brightness -= 1;
                        updateMenu = true;
                    }
                    turns++;
                }
            }
            if (updateMenu) { 
                // Update LCD 
                menuChangeBrightness(fsm->brightness);
                lcdSetDisplayBrightness(fsm->brightness);
                updateMenu = false;
            }
            break;
            
        case S_SEL_CONTRAST:
            if (updateMenu) {
                menuSelectContrast(fsm->brightness, fsm->contrast);
                updateMenu = false;
            }
            break;
            
        case S_CHA_CONTRAST:
            while (turns != 0) {
                if (turns > 0) {
                    if (fsm->contrast < 50) {
                        fsm->contrast += 1;
                        updateMenu = true;
                    }
                    turns--;
                } else {
                    if (fsm->contrast > 1) {
                        fsm->contrast -= 1;
                        updateMenu = true;
                    }
                    turns++;
                }
            }
            if (updateMenu) { 
                // Update LCD 
                menuChangeContrast(fsm->contrast);
                lcdSetDisplayContrast(fsm->contrast);
                updateMenu = false;
            }
            break;
            
        default:
            break;
    }
}


void errorFsmCalculateNextState(
        volatile ErrorFSM_t * fsm, 
        int16_t turns, 
        Button_e buttonState) {
    
     fsm->nextState = fsm->currentState;
    
    // Find next state
    switch (fsm->currentState) {
        case E_INIT:
            switch (fsm->lastError.source) {
                default:
                case ES_UNK:
                    fsm->nextState = E_UNK_ERROR;
                    break;
                case ES_I2C:
                    fsm->nextState = E_I2C_ERROR;
                    break;
                case ES_SYS:
                    fsm->nextState = E_SYS_ERROR;
                    break;
            }
            break;
            
        case E_SYS_ERROR:
        case E_I2C_ERROR:
        case E_UNK_ERROR:
            if (buttonState == Held) {
                fsm->nextState = E_RESET;
            } else if (buttonState == DoubleClicked) {
                fsm->nextState = E_STOP;
            }
            break;
            
        case E_RESET:
            fsm->nextState = E_STOP;
            break;
            
        case E_STOP:
            fsm->nextState = E_INIT;
            break;
            
        default:
            break;
    }
    
}

void errorFsmHandleState(volatile ErrorFSM_t * fsm) {
    
    static const char *title;
    static const char *message;
    
     switch (fsm->currentState) {
        case E_INIT:
            updateMenu = true;
            break;
            
        case E_UNK_ERROR:
            if (updateMenu) {
                title = ERROR_TITLE_UNK;
                message = "";
                menuShowError(title, message);
                updateMenu = false;
            }
            break;
            
        case E_I2C_ERROR:
            if (updateMenu) {
                title = ERROR_TITLE_I2C;
                switch (fsm->lastError.code) {
                    case I2C_NOK: message = E_MSG_I2C_NOK; break;
                    case I2C_OVERFLOW: message = E_MSG_I2C_OVERFLOW; break;
                    case I2C_COLLISION: message = E_MSG_I2C_COLLISION; break;
                    case I2C_NO_ADR_ACK: message = E_MSG_I2C_NO_ADR_ACK; break;
                    case I2C_NO_DATA_ACK: message = E_MSG_I2C_NO_DATA_ACK; break;
                    case I2C_UNEXPECTED_DATA: message = E_MSG_I2C_UNEXPECTED_DATA; break;
                    case I2C_UNEXPECTED_ADR: message = E_MSG_I2C_UNEXPECTED_ADR; break;
                    case I2C_STILL_BUSY: message = E_MSG_I2C_STILL_BUSY; break;
                    case I2C_TIMEOUT: message = E_MSG_I2C_TIMEOUT; break;
                }
                menuShowError(title, message);
                updateMenu = false;
                
                if (DEBUG_I2C) printI2CError(fsm->lastError.code);
            }
            break;
            
        case E_SYS_ERROR:
            if (updateMenu) {
                title = ERROR_TITLE_SYS;
                message = "";
                menuShowError(title, message);
                updateMenu = false;
            }
            break;
            
         case E_RESET:
             sysReset(10);
             break;
            
        case E_STOP:
            break;
    }
}


/*******************************************************************************
 *          MAIN PROGRAM
 ******************************************************************************/
int main(void) {

    initialize();
    DelayMs(100);
    
    //uartInit(&putCommand);
    glcdInit();
    LED1 = 1;


    DelayMs(100);
    glcdSelectMenu(0, true);
    glcdWriteMenu(0);

    while(1) {

        /* glcdSelectMenu(0, true); */
        /* glcdWriteMenu(0); */
        /* LED1 = !LED1; */
        /* DelayMs(2000); */
        /* glcdSelectMenu(0, false); */
        /* glcdWriteMenu(0); */
        /* LED1 = !LED1; */
        /* DelayMs(2000); */
    }
    return 0;

    /*
    encDriverInit();
    suppliesInit(&supplyStatus, &onError);
    menuInit(&putCommand);
    DelayMs(100);
    
    // Initial values
    mainFsm.execute = false;
    mainFsm.currentState = M_INIT;
    mainFsm.nextState = M_INIT;
    mainFsm.prescale = 0;
    mainFsm.waitCnt = 0;
    
    calibrateFsm.currentState = C_INIT;
    calibrateFsm.nextState = C_INIT;
    calibrateFsm.desiredVoltage = 0;
    calibrateFsm.calibrationCount = 0;
    
    settingsFsm.currentState = S_INIT;
    settingsFsm.nextState = S_INIT;
    settingsFsm.brightness = 7;
    settingsFsm.contrast = 30;
    
    errorFsm.currentState = E_INIT;
    errorFsm.nextState = E_INIT;
    errorFsm.lastError.hasError = false;
    errorFsm.lastError.source = ES_UNK;
    errorFsm.lastError.code = 0;
    
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
            
            mainFsmCalculateNextState(&mainFsm, encTurns, encButtonState);
            
            if (DEBUG_FSM && (mainFsm.currentState != mainFsm.nextState)) {
                printf("M S%d>S%d\n", mainFsm.currentState, mainFsm.nextState);
            }
            if (DEBUG_FSM && (calibrateFsm.currentState != calibrateFsm.nextState)) {
                printf("C S%d>S%d\n", calibrateFsm.currentState, calibrateFsm.nextState);
            }
            if (DEBUG_FSM && (settingsFsm.currentState != settingsFsm.nextState)) {
                printf("X S%d>S%d\n", settingsFsm.currentState, settingsFsm.nextState);
            }
            if (DEBUG_FSM && (errorFsm.currentState != errorFsm.nextState)) {
                printf("E S%d>S%d\n", errorFsm.currentState, errorFsm.nextState);
            }
            
            mainFsmHandeState(&mainFsm, &settingsFsm, encTurns);
            
            if (DEBUG_FSM && (previousStatus != supplyStatus.value)) {
                printStatus(supplyStatus);
                previousStatus = supplyStatus.value;
            }
            
            // Update FSMs
            mainFsm.currentState = mainFsm.nextState;
            calibrateFsm.currentState = calibrateFsm.nextState;
            settingsFsm.currentState = settingsFsm.nextState;
            errorFsm.currentState = errorFsm.nextState;
            
        }
        
    }
    return 0;
    */
}


/*******************************************************************************
 *          INTERRUPTS AND EVENTS
 ******************************************************************************/
void __attribute__ ( (interrupt, no_auto_psv) ) _T4Interrupt(void) {
    if (_T4IF) {
        heartBeat();
        _T4IF = 0; // Clear interrupt
    }
}


bool putCommand(Command_t command) {
    return true;
}

void onError(Error_t e) {
    if (!errorFsm.lastError.hasError) { // Still working on previous error..
        errorFsm.lastError.hasError = true;
        errorFsm.lastError.source = e.source;
        errorFsm.lastError.code = e.code;
    }
}

void onSysError(int16_t e) {
    Error_t error = {true, ES_SYS, e};
    onError(error);
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

