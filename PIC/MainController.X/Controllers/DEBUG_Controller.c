#include "DEBUG_Controller.h"

/*******************************************************************************
 *              DEFINITIONS
 ******************************************************************************/


/*******************************************************************************
 *              PROTOTYPES
 ******************************************************************************/

/*******************************************************************************
 *              VARIABLES
 ******************************************************************************/

/*******************************************************************************            
 *              FUCTIONS
 ******************************************************************************/

void dbgInit() {
    
}

void dbgPrintReady() {
    if (DEBUG_FSM) {
        printf("Ready\n");
    }
}

void dbgPrintSupplyStatus(SupplyStatus_t status) {
    if (DEBUG_SPL) {
        printf("STATUS: \n");
        printf(" - Code: %d\n", status.statusCode);
        printf(" - Error: %d\n", status.errorCode);
        printf(" - Clip: %d\n", status.currentClip);
        printf(" - Calib: %d\n", status.calibrationSt);
        printf(" - O EN: %d\n", status.outputEnabled);
        printf(" - C EN: %d\n", status.calibrateEnabled);
        printf(" - P EN: %d\n", status.pidEnabled);
    }
}

void dbgPrintI2CError(int16_t errorCode) {
    if (DEBUG_FSM) {
        printf("I2C ERROR: ");
        switch (errorCode) {
            default:
                printf("UNK: %d\n", errorCode);
                break;
            case I2C_OK: printf("OK\n");
                break;
            case I2C_NOK: printf("NOK\n");
                break;
            case I2C_OVERFLOW: printf("OVERFLOW\n");
                break;
            case I2C_COLLISION: printf("COLLISION\n");
                break;
            case I2C_NO_ADR_ACK: printf("NO_ADR_ACK\n");
                break;
            case I2C_NO_DATA_ACK: printf("NO_DATA_ACK\n");
                break;
            case I2C_UNEXPECTED_DATA: printf("UNEXPECTED_DATA\n");
                break;
            case I2C_UNEXPECTED_ADR: printf("UNEXPECTED_ADR\n");
                break;
            case I2C_STILL_BUSY: printf("STILL_BUSY\n");
                break;
            case I2C_TIMEOUT: printf("TIMEOUT\n");
                break;
        }
    }
}

void  dbgPrintI2cData(uint16_t i, uint8_t data) {
    if (DEBUG_I2C) {
        printf(" - %d = %d\n", i, data);
    }
}