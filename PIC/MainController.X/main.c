#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "Utils.h"
#include "Settings.h"
#include "Drivers/SYSTEM_Driver.h"
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

// Callback
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



/*******************************************************************************
 *          MAIN PROGRAM
 ******************************************************************************/
int main(void) {

    initialize();
    UART1_TX_Pin = 1;
    UART2_TX_Pin = 1;
    DelayMs(100);
    DelayMs(100);
    DelayMs(100);
    DelayMs(100);
   
    fsmInit();

    while(1) {

        if (fsmShouldExecute()) {
            fsmExecute();
        }
    }
    return 0;
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
