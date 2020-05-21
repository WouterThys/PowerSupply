#include "DEBUG_Controller.h"
#include "SUPPLIES_Controller.h"

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

void dbgPrintSupplyStatus(uint8_t id, SupplyStatus_t status) {
    if (DEBUG_SPL) {
        printf("SPL STATUS: %d\n", id);
        printf(" - Code: %d\n", status.status_code);
        printf(" - Error: %d\n", status.error_code);
        printf(" - Clip: %d\n", status.current_clip);
        printf(" - Calib: %d\n", status.calibration_state);
        printf(" - O EN: %d\n", status.output_enabled);
        printf(" - C EN: %d\n", status.calibrate_enabled);
        printf(" - P EN: %d\n", status.pid_enabled);
    }
}

void dbgPrintSupplyMeasurements(uint8_t id, uint16_t v, uint16_t i, uint16_t t) {
    if (DEBUG_SPL) {
        printf("SPL DATA: %d\n", id);
        printf(" - V: %d\n", v);
        printf(" - I: %d\n", i);
        printf(" - T: %d\n", t);
    }
}

void dbgPrintI2CError(uint16_t id, int16_t errorCode) {
    if (DEBUG_FSM) {
        printf("I2C %d ERROR: ", id);
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

void dbgPrintLcdCallback(uint8_t data) {
    if (DEBUG_LCD) {
        printf("LCD: %d\n", data);
    }
}