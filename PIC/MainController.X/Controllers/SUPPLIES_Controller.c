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

// Callback
static void (*onSupplyError)(Error_t error);

//
static void preparePackage(i2cPackage_t * package, uint8_t address, uint8_t command, uint8_t length, uint16_t * data);
static bool i2cCheckState(i2cPackage_t data);


/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static uint16_t dataArray[7];
static uint16_t digital; // Converted value to write to supplies
static i2cPackage_t i2cPackage;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          I²C
 ******************************************************************************/
static bool i2cCheckState(i2cPackage_t data) {
    if (data.status < I2C_IDLE) {
        Error_t error = {true, ES_I2C, data.status};
        (*onSupplyError)(error);
        i2cDriverReset(); // Clear errors
        return false;
    }
    return true;
}

static void preparePackage(i2cPackage_t * package, uint8_t address, uint8_t command, uint8_t length, uint16_t * data) {
    package->address = address;
    package->command = command;
    package->status = I2C_IDLE;
    package->length = length;
    package->data = data;
}

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/
void splInit(void (*onError)(Error_t error)) {

    // Event
    onSupplyError = onError;

    // Initial values
    digital = 0;
    i2cPackage.address = 0;
    i2cPackage.command = 0;
    i2cPackage.length = 0;

    // I²C
    i2cDriverInit();
    i2cDriverEnable(true);
}

void splWriteVoltage(const SupplyData_t * data) {
    if (data == NULL) return;

    // Convert to digital value
    digital = voltageToDigital(data->set_voltage.value);

    // Prepare
    preparePackage(&i2cPackage, data->i2c_address, I2C_COM_SET_V, 1, &digital);

    // Write
    i2cDriverWrite(&i2cPackage);

    // Check
    i2cCheckState(i2cPackage);
}

void splWriteCurrent(const SupplyData_t * data) {
    if (data == NULL) return;

    // Convert to digital value
    digital = currentToDigital(data->set_current.value);

    // Prepare
    preparePackage(&i2cPackage, data->i2c_address, I2C_COM_SET_I, 1, &digital);
    
    // Write
    i2cDriverWrite(&i2cPackage);

    // Check
    i2cCheckState(i2cPackage);
}

void splSetCalibration(CalibrationFSM_t fsm) {

    uint16_t data[2] = {fsm.saveState, fsm.calibrationCount};

    // Send
    //i2cPackage.address = data->i2c_address;
    i2cPackage.length = 2;
    i2cPackage.command = I2C_COM_CAL_STATE;
    i2cPackage.data = &data[0];

    i2cDriverWrite(&i2cPackage);
    i2cCheckState(i2cPackage);
}

void splUpdateData(SupplyStatus_t * status, SupplyData_t * data) {
    if (data == NULL) return;

    // Prepare
    preparePackage(&i2cPackage, data->i2c_address, I2C_COM_MSR_V, 5, &dataArray[I2C_COM_MSR_V]);

    // Read
    i2cDriverRead(&i2cPackage);

    // Update status
    status->value = dataArray[I2C_COM_STATUS];

    // Check and update
    if (i2cCheckState(i2cPackage)) {

        // Update values
        if (data->msr_voltage.value != dataArray[I2C_COM_MSR_V]) {
            data->msr_voltage.value = dataArray[I2C_COM_MSR_V];
            data->msr_voltage.changed = true;
        }

        if (data->msr_current.value != dataArray[I2C_COM_MSR_I]) {
            data->msr_current.value = dataArray[I2C_COM_MSR_I];
            data->msr_current.changed = true;
        }

        if (data->msr_temperature.value != dataArray[I2C_COM_MSR_T]) {
            data->msr_temperature.value = dataArray[I2C_COM_MSR_T];
            data->msr_temperature.changed = true;
        }
    }
}
