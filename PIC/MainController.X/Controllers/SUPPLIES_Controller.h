#ifndef SUPLIES_CONTROLLER_H
#define	SUPLIES_CONTROLLER_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "../../Common/CommonData.h"


typedef struct {
    uint16_t value   : 15;          /* Value                                  */
    uint16_t changed : 1;           /* Value has changed flag                 */
} SupplyValue_t;

typedef struct {
    SupplyValue_t setVoltage;       /* Variable voltage                       */
    SupplyValue_t setCurrent;       /* Maximum current                        */
    SupplyValue_t msrVoltage;       /* Measured output voltage                */
    SupplyValue_t msrCurrent;       /* Measured output current                */
    SupplyValue_t msrTemperature;   /* Measured temperature of the LT3081     */
} SupplyData_t;

/**
 * Initialize supply connection:
 *  - Set data packets
 *  - Initialize I2C 
 * @param status
 * @param onError
 */
void suppliesInit(SupplyStatus_t * status, void (*onError)(Error_t error));

/**
 * Set voltage of supply board, and write too I2C
 * @param voltage
 */
void splSetVoltage(uint16_t voltage);

/**
 * Set current of supply board, and write too I2C
 * @param current
 */
void splSetCurrent(uint16_t current);

/**
 * Set status of supply board
 * @param status
 */
void splSetStatus(SupplyStatus_t status);

/**
 * Read the measured data from supply board over I2C
 */
void splUpdateMeasuremnets();

/**
 * Update data packet with data within this supply controller
 * @param data
 */
void splUpdateData(SupplyData_t * data);

/**
 * Update calibration state on supply board
 * @param fsm
 */
void splSetCalibration(CalibrationFSM_t fsm);


#endif	/* XC_HEADER_TEMPLATE_H */