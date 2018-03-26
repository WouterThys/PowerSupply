/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef I2C_DRIVER_H
#define	I2C_DRIVER_H

#include <xc.h> // include processor files - each processor file is guarded. 

#define I2C_MWRITE          3
#define I2C_MREAD           2
#define I2C_OK              1
#define I2C_NOK             -1     
#define I2C_OVERFLOW        -2
#define I2C_COLLISION       -3
#define I2C_NO_ADR_ACK      -4
#define I2C_NO_DATA_ACK     -5
#define I2C_UNEXPECTED_DATA -6
#define I2C_UNEXPECTED_ADR  -7
#define I2C_STILL_BUSY      -8

struct I2C_Data {
    uint8_t address;    /* Address (7-bit) of the slave                       */
    uint8_t command;    /* Command (8-bit) send from M -> S                   */
    int8_t  status;     /* Status of R/W                                      */
    uint8_t data1;      /* First data (8-bit) send from S -> M                */
    uint8_t data2;      /* Second data (8-bit) send from S -> M               */
};
typedef struct I2C_Data i2cData_t;

/**
 * 
 */
void i2cDriverInitMaster();

/**
 * 
 * @param address
 * @param onI2cAnswer
 * @param onI2cReadDone
 */
void i2cDriverInitSlave(uint16_t address, void (*onI2cAnswer)(i2cData_t * data), void (*onI2cReadDone)(i2cData_t data));

/**
 * 
 * @param enable 
 */
void i2cDriverEnable(bool enable);

/**
 * 
 */
void i2cDriverReset();

/**
 * Write data to slave.
 * @param data: i2cData_t containing address and data for and from the slave
 */
void i2cDriverMasterWrite(i2cData_t *data);

/**
 * Read data from slave
 * @param data
 */
void i2cDriverMasterRead(i2cData_t *data);

/**
 * Read data from master, after the Master has initiated a transaction.
 * @param data: i2cData_t containing data from master and answer from slave.
 */
void i2cDriverSlaveRead(i2cData_t *data);



#endif	/* XC_HEADER_TEMPLATE_H */
