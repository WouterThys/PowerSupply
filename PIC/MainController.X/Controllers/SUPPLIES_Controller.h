#ifndef SUPLIES_CONTROLLER_H
#define	SUPLIES_CONTROLLER_H

#include <xc.h> // include processor files - each processor file is guarded. 
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "../Utils.h"
#include "../Settings.h"
#include "../Drivers/I2C_Driver.h"
#include "../Drivers/SYSTEM_Driver.h"
#include "../../Common/CommonData.h"

typedef struct {
    uint16_t value   : 15;          /* Value                                  */
    uint16_t changed : 1;           /* Value has changed flag                 */
} SupplyValue_t;

typedef struct {
    uint16_t      supply_id : 8;    /* Id of the supply                       */
    uint16_t      i2c_address;      /* I2C address of the supply              */   
    SupplyValue_t set_voltage;      /* Variable voltage                       */
    SupplyValue_t set_current;      /* Maximum current                        */
    SupplyValue_t msr_voltage;      /* Measured output voltage                */
    SupplyValue_t msr_current;      /* Measured output current                */
    SupplyValue_t msr_temperature;  /* Measured temperature of the LT3081     */
} SupplyData_t;

/**
 * Initialize supply connection:
 *  - Initialize I2C 
 * @param onError
 */
void splInit(void (*onError)(Error_t error));

/**
 * Set voltage of supply board, and write too I2C
 * @param data
 */
void splWriteVoltage(const SupplyData_t * data);

/**
 * Set current of supply board, and write too I2C
 * @param data
 */
void splWriteCurrent(const SupplyData_t * data);

/**
 * Set status of supply board
 * @param status
 */
void splSetStatus(SupplyStatus_t status);

/**
 * Update data packet with data within this supply controller
 * @param status
 * @param data
 */
void splUpdateData(SupplyStatus_t * status, SupplyData_t * data);

/**
 * Update calibration state on supply board
 * @param fsm
 */
void splSetCalibration(CalibrationFSM_t fsm);


#endif	/* XC_HEADER_TEMPLATE_H */