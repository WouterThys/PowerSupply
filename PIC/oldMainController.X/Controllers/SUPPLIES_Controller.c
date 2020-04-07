#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "../utils.h"
#include "../Settings.h"
#include "../Drivers/I2C_Driver.h"
#include "../Drivers/SYSTEM_Driver.h"

#include "SUPPLIES_Controller.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/


/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          LOCAL FUNCTION DEFINES
 ******************************************************************************/

/* Event function pointers */
static void (*onSupplyError)(Error_t error);

static bool i2cCheckState(i2cPackage_t data);


/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static uint16_t dataArray[7];
static uint16_t * setVoltage;
static uint16_t * setCurrent;
static uint16_t * msrVoltage;
static uint16_t * msrCurrent;
static uint16_t * msrTemperature;
static uint16_t * msrCurrent_;

static SupplyStatus_t * status;
static i2cPackage_t i2cPackage;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          I²C
 ******************************************************************************/
bool i2cCheckState(i2cPackage_t data) {
    if (data.status < I2C_OK) {
        Error_t error = { true, ES_I2C, data.status };
        (*onSupplyError)(error);
        return false;
    } 
    return true;
}

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/
void suppliesInit(SupplyStatus_t * s, void (*onError)(Error_t error)) {
    
    // Event
    onSupplyError = onError;
    
    // Initialize pointers to buffer
    setVoltage = &dataArray[I2C_COM_SET_V];
    setCurrent = &dataArray[I2C_COM_SET_I];
    msrVoltage = &dataArray[I2C_COM_MSR_V];
    msrCurrent = &dataArray[I2C_COM_MSR_I];
    msrTemperature = &dataArray[I2C_COM_MSR_T];
    msrCurrent_ = &dataArray[I2C_COM_MSR_I_];
    status = s;
    
    // Initial values
    i2cPackage.address = I2C_ADDRESS;
    i2cPackage.command = 0;
    i2cPackage.length = 0;
    i2cPackage.data = setVoltage;
    
    // I²C
    i2cDriverInit();
    i2cDriverEnable(true);
    
    printf("I2C ready \n");
}

//void splSetStatus(SupplyStatus_t s) {
//    // DONT SET LOCAL STATUS NOW, BUT WAIT WHEN READ BACK FROM SLAVE!!!
//    
//    // Send
//    i2cPackage.length = 1;
//    i2cPackage.command = I2C_COM_STATUS;
//    i2cPackage.data = &s.value;
//    
//    i2cDriverWrite(&i2cPackage);
//    i2cCheckState(i2cPackage);
//}

void splSetVoltage(uint16_t voltage) {

    *setVoltage = voltage;
    
    // Send
    i2cPackage.length = 1;
    i2cPackage.command = I2C_COM_SET_V;
    i2cPackage.data = setVoltage;
    
    i2cDriverWrite(&i2cPackage);
    //i2cCheckState(i2cPackage);
    
}

void splSetCurrent(uint16_t current) {
    
    *setCurrent = current;
    
    // Send
    i2cPackage.length = 1;
    i2cPackage.command = I2C_COM_SET_I;
    i2cPackage.data = setCurrent;
    
    i2cDriverWrite(&i2cPackage);
    i2cCheckState(i2cPackage);
    
}

void splSetCalibration(CalibrationFSM_t fsm) {
    
    uint16_t data[2] = { fsm.saveState, fsm.calibrationCount };
    
    // Send
    i2cPackage.length = 2;
    i2cPackage.command = I2C_COM_CAL_STATE;
    i2cPackage.data = &data[0];
    
    i2cDriverWrite(&i2cPackage);
    i2cCheckState(i2cPackage);
}

void splUpdateMeasuremnets() {
    
    i2cPackage.length = 5; // Voltage, current, temperature, current_ and status
    i2cPackage.command = I2C_COM_MSR_V; // First thing to measure
    i2cPackage.data = &dataArray[I2C_COM_MSR_V];
    
    i2cDriverRead(&i2cPackage);
    i2cCheckState(i2cPackage);
    
    // Update status
    status->value = dataArray[I2C_COM_STATUS];
}


void splUpdateData(SupplyData_t * data) {
    
    if (data->msrVoltage.value != *msrVoltage) {
        data->msrVoltage.value = *msrVoltage;
        data->msrVoltage.changed = true;
    }
    
    if (data->msrCurrent.value != *msrCurrent) {
        data->msrCurrent.value = *msrCurrent;
        data->msrCurrent.changed = true;
    }
    
    if (data->msrTemperature.value != *msrTemperature) {
        data->msrTemperature.value = *msrTemperature;
        data->msrTemperature.changed = true;
    }
    
}
