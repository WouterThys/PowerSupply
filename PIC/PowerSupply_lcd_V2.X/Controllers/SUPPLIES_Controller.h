#ifndef SUPLIES_CONTROLLER_H
#define	SUPLIES_CONTROLLER_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "../Drivers/I2C_Driver.h"

typedef struct {
    bool changed;
    uint16_t setVoltage; // Variable voltage 
    uint16_t setCurrent; // Maximum current
    uint16_t msrVoltage; // Measured output voltage
    uint16_t msrCurrent; // Measured output current
    uint16_t msrTemperature; // Measured temperature of the LT3081
} SupplyData_t;


void suppliesInit();

void setVoltage(uint16_t voltage);

void setCurrent(uint16_t current);

void getVarData(SupplyData_t * data);
void get5V0Data(SupplyData_t * data);
void get3V3Data(SupplyData_t * data);


#endif	/* XC_HEADER_TEMPLATE_H */