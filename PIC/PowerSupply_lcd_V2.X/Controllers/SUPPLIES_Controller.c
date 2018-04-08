#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "../utils.h"
#include "../Settings.h"
#include "../Drivers/SYSTEM_Driver.h"
#include "../../Common/COM_Settings.h"

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
static bool i2cCheckState(i2cData_t data);


/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static i2cData_t i2cVar;
static i2cData_t i2c5V0;
static i2cData_t i2c3V3;

static int8_t i2cError = I2C_OK;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
void setVoltage(uint16_t voltage) {
    split(voltage, &i2cVar.data1, &i2cVar.data2);
    i2cVar.command = COM_SET_V;
    i2cDriverMasterWrite(&i2cVar);
    i2cCheckState(i2cVar);
}

void setCurrent(uint16_t current) {
    split(current, &i2cVar.data1, &i2cVar.data2);
    i2cVar.command = COM_SET_I;
    i2cDriverMasterWrite(&i2cVar);
    i2cCheckState(i2cVar);
}

/*******************************************************************************
 *          I²C
 ******************************************************************************/
bool i2cCheckState(i2cData_t data) {
    //if (data.status != i2cError) {
        i2cError = data.status;
        if (DEBUG & DEBUG_I2C) {
            switch(i2cError) {
                default: 
                    printf("I2C_OK\n"); break;
                    break;
                case I2C_NOK: printf("I2C_NOK\n"); break;
                case I2C_OVERFLOW: printf("I2C_OVERFLOW\n"); break;
                case I2C_COLLISION: printf("I2C_COLLISION\n"); break;
                case I2C_NO_ADR_ACK: printf("I2C_NO_ADR_ACK\n"); break;
                case I2C_NO_DATA_ACK: printf("I2C_NO_DATA_ACK\n"); break;
                case I2C_UNEXPECTED_DATA: printf("I2C_UNEXPECTED_DATA\n"); break;
                case I2C_UNEXPECTED_ADR: printf("I2C_UNEXPECTED_ADR\n"); break;
                case I2C_STILL_BUSY: printf("I2C_STILL_BUSY\n"); break;
                case I2C_TIMEOUT: printf("I2C_TIMEOUT\n"); break;
            }
        }
    //}
    
    if (i2cError < I2C_OK) {
        //LED1 = 1;
        i2cDriverReset();
        return false;
    } else {
        //LED1 = 0;
        return true; 
    }
}

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/
void suppliesInit() {
    
    i2cVar.address = ADDR_VAR;
    i2c5V0.address = ADDR_5V0;
    i2c3V3.address = ADDR_3V3;
    
    i2cDriverInitMaster();
    i2cDriverEnable(true);
}


void getVarData(SupplyData_t * data) {
    uint16_t value = 0;
    
    i2cVar.command = COM_GET_V;
    i2cDriverMasterRead(&i2cVar);
    if (i2cCheckState(i2cVar)) {
        concatinate(i2cVar.data1, i2cVar.data2, &value);   
        if (value != data->msrVoltage.value) {
            data->msrVoltage.value = value;
            data->msrVoltage.changed = true;
        }
    }
    DelayUs(10);
    
    i2cVar.command = COM_GET_I;
    i2cDriverMasterRead(&i2cVar);
    if (i2cCheckState(i2cVar)) {
        concatinate(i2cVar.data1, i2cVar.data2, &value);
        if (value != data->msrCurrent.value) {
            data->msrCurrent.value = value;
            data->msrCurrent.changed = true;
        }
    }
    DelayUs(10);
    
    i2cVar.command = COM_GET_T;
    i2cDriverMasterRead(&i2cVar);
    if (i2cCheckState(i2cVar)) {
        concatinate(i2cVar.data1, i2cVar.data2, &value);
        if (value != data->msrTemperature.value) {
            data->msrTemperature.value = value;
            data->msrTemperature.changed = true;
        }
    }
}


void get5V0Data(SupplyData_t * data) {
//    i2c5V0.command = COM_GET_V;
//    i2cDriverMasterRead(&i2c5V0);
//    if (i2cCheckState(i2c5V0)) {
//        concatinate(i2c5V0.data1, i2c5V0.data2, &data->msrVoltage);   
//    }
//    
//    i2c5V0.command = COM_GET_I;
//    i2cDriverMasterRead(&i2c5V0);
//    if (i2cCheckState(i2c5V0)) {
//        concatinate(i2c5V0.data1, i2c5V0.data2, &data->msrCurrent);   
//    }
//    
//    i2c5V0.command = COM_GET_T;
//    i2cDriverMasterRead(&i2c5V0);
//    if (i2cCheckState(i2c5V0)) {
//        concatinate(i2c5V0.data1, i2c5V0.data2, &data->msrTemperature);   
//    }
}


void get3V3Data(SupplyData_t * data) {
//    i2c3V3.command = COM_GET_V;
//    i2cDriverMasterRead(&i2c3V3);
//    if (i2cCheckState(i2c3V3)) {
//        concatinate(i2c3V3.data1, i2c3V3.data2, &data->msrVoltage);   
//    }
//    
//    i2c3V3.command = COM_GET_I;
//    i2cDriverMasterRead(&i2c3V3);
//    if (i2cCheckState(i2c3V3)) {
//        concatinate(i2c3V3.data1, i2c3V3.data2, &data->msrCurrent);   
//    }
//    
//    i2c3V3.command = COM_GET_T;
//    i2cDriverMasterRead(&i2c3V3);
//    if (i2cCheckState(i2c3V3)) {
//        concatinate(i2c3V3.data1, i2c3V3.data2, &data->msrTemperature);   
//    }
}