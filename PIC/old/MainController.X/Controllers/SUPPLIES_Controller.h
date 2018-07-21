#ifndef SUPLIES_CONTROLLER_H
#define	SUPLIES_CONTROLLER_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "../Drivers/I2C_Driver.h"

typedef struct {
    uint16_t value   : 15; // Value
    uint16_t changed : 1; // Value has changed
} SupplyValue_t;

typedef struct {
    SupplyValue_t setVoltage;  // Variable voltage 
    SupplyValue_t setCurrent;  // Maximum current
    SupplyValue_t msrVoltage; // Measured output voltage
    SupplyValue_t msrCurrent; // Measured output current
    SupplyValue_t msrTemperature; // Measured temperature of the LT3081
    SupplyValue_t status;
} SupplyData_t;


void suppliesInit();

void splSetVoltage(uint16_t voltage);
void splSetCurrent(uint16_t current);

void splUpdateMeasuremnets();
void splUpdateData(SupplyData_t * data);


#endif	/* XC_HEADER_TEMPLATE_H */