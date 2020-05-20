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
static uint16_t digital; // Converted value to write to supplies
static i2cPackage_t i2cPackage;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          I²C
 ******************************************************************************/
bool i2cCheckState(i2cPackage_t data) {
    if (data.status < I2C_IDLE) {
        Error_t error = {true, ES_I2C, data.status};
        (*onSupplyError)(error);
        return false;
    }
    return true;
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
    i2cPackage.address = data->i2c_address;
    i2cPackage.length = 1;
    i2cPackage.command = I2C_COM_SET_V;
    i2cPackage.data = &digital;

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
    i2cPackage.address = data->i2c_address;
    i2cPackage.length = 1;
    i2cPackage.command = I2C_COM_SET_I;
    i2cPackage.data = &digital;

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
    i2cPackage.address = data->i2c_address;
    i2cPackage.length = 5; // Voltage, current, temperature, current_ and status
    i2cPackage.command = I2C_COM_MSR_V; // First thing to measure
    i2cPackage.data = &dataArray[I2C_COM_MSR_V];

    // Read
    i2cDriverRead(&i2cPackage);
    if (i2cCheckState(i2cPackage)) {

        // Update status
        status->value = dataArray[I2C_COM_STATUS];

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
