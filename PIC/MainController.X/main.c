#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <string.h>
#include <math.h>

#include "Utils.h"
#include "Settings.h"
#include "Drivers/SYSTEM_Driver.h"
#include "Drivers/I2C_Driver.h"
#include "Drivers/ENC_Driver.h"
#include "Drivers/UART_Driver.h"

#include "Controllers/SUPPLIES_Controller.h"
#include "Controllers/GLCD_Controller.h"
#include "Controllers/FSM_Controller.h"


/*******************************************************************************
 *          TODO
 *******************************************************************************
 * Old and handy:
  
 * splUpdateData(&supplyData); // Get measurement data
  
 *  // Default values
    supplyData.setVoltage.value = DEFAULT_V;
    supplyData.setCurrent.value = DEFAULT_I;
    splSetVoltage(supplyData.setVoltage.value);
    splSetCurrent(supplyData.setCurrent.value);
 
 *  encDriverService();

 * */
    
// Setup timer
/*******************************************************************************
 *          DEFINES
 ******************************************************************************/       


/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/


/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize();
static void printStatus(SupplyStatus_t status);
static void printI2CError(int16_t errorCode);

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

/*******************************************************************************
 *          MAIN PROGRAM
 ******************************************************************************/
int main(void) {

    initialize();
    DelayMs(100);
   
    // TODO: Move to init phase of FSM 
    glcdInit();
    //encDriverInit();
    //fsmInit();

//    DelayMs(100);
//    LED1 = 1;
//    glcdSelectMenu(1, true);
//    glcdSetVoltageState(1, SELECT_V);
//    glcdSetVoltageRead(1, "8,24 V");
//    glcdSetCurrentRead(1, "POG2 A");
//    glcdWriteMenu(1); 
//    LED1 = 0;

    //printf("Start\n");
    
   
    
    uartDriverWriteByte(0xFE);
    uartDriverWriteByte(0x58);

    while(1) {


        if (fsmShouldExecute()) {
            //fsmExecute();
            LED1 = !LED1;
        }       
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


void onError(Error_t e) {
//    if (!errorFsm.lastError.hasError) { // Still working on previous error..
//        errorFsm.lastError.hasError = true;
//        errorFsm.lastError.source = e.source;
//        errorFsm.lastError.code = e.code;
//    }
}

void onSysError(int16_t e) {
    Error_t error = {true, ES_SYS, e};
    onError(error);
}
