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
#include "../Settings.h"

#define MAX_TIMEOUT   20
#define MAX_RETRY     3
#define MAX_BUFFER    16

#define I2C_MWRITE          3
#define I2C_MREAD           2
#define I2C_OK              1
#define I2C_IDLE            0
#define I2C_NOK             -1     
#define I2C_OVERFLOW        -2
#define I2C_COLLISION       -3
#define I2C_NO_ADR_ACK      -4
#define I2C_NO_DATA_ACK     -5  /* Happens when other device is not there     */
#define I2C_UNEXPECTED_DATA -6
#define I2C_UNEXPECTED_ADR  -7
#define I2C_STILL_BUSY      -8  /* Writing or reading still in progress       */
#define I2C_TIMEOUT         -9

typedef struct {
    uint8_t address;    /* Address (7-bit) of the slave                       */
    uint8_t command;    /* Command (8-bit) send from M -> S                   */
    int16_t  status;    /* Status of R/W                                      */
    uint16_t length;    /* Length of data buffer                              */
    uint16_t * data;    /* The data                                           */
    
} i2cPackage_t;

#ifdef I2C_MASTER
void i2cDriverInit();
#endif

#ifdef I2C_SLAVE
void i2cDriverInit(i2cPackage_t * data, void (*onI2cEvent)(i2cPackage_t data)); 
#endif

/**
 * 
 * @param enable 
 */
void i2cDriverEnable(bool enable);

/**
 * 
 */
void i2cDriverReset();

#ifdef I2C_MASTER
/**
 * Write data to slave.
 * @param data: i2cData_t containing address and data for and from the slave
 */
void i2cDriverWrite(i2cPackage_t *data);
#endif

#ifdef I2C_MASTER
/**
 * Read data from slave
 * @param data
 */
void i2cDriverRead(i2cPackage_t *data);
#endif


#endif	/* XC_HEADER_TEMPLATE_H */