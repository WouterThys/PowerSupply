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


#define I2C_MASTER_MODE     1
#define I2C_SLAVE_MODE      0

#define I2C_WRITE           0
#define I2C_READ            1

#define I2C_OK              1
#define I2C_NOK             -1     
#define I2C_OVERFLOW        -2
#define I2C_COLLISION       -3

#define I2C_ADDRESS_MASK    0xFE    /* Mask for fist 7 bits in address byte   */
#define I2C_RW_MASK         0x01    /* Mask for write or read indicator       */

#define I2C_STATUS_MASK     0x0F    /* Mask for 4-bit status                  */
#define I2C_FIRST_DATA_MASK 0x0F    /* Mask for 4-bit first data              */
/*

 */

extern bool I2C_ReadyToRead;

struct I2C_Data {
    uint8_t address;    /* Address (7-bit) of the slave                       */
    uint8_t command;    /* Command (8-bit) send from M -> S                   */
    uint8_t status;     /* Status (4-bit) send from S -> M                    */
    uint8_t data1;      /* First data (8-bit) send from S -> M                */
    uint8_t data2;      /* Second data (8-bit) send from S -> M               */
};
typedef struct I2C_Data i2cData_t;

/**
 * 
 */
void D_I2C_InitMaster();

/**
 * 
 * @param address
 */
void D_I2C_InitSlave(uint16_t address);

/**
 * 
 * @param enable 
 */
void D_I2C_Enable(bool enable);

/**
 * 
 */
void D_I2C_Reset();

/**
 * Write data to slave.
 * @param data: i2cData_t containing address and data for and from the slave
 * @return Positive if OK
 */
int16_t D_I2C_Write(i2cData_t *data);

/**
 * Read data from master, after the Master has initiated a transaction.
 * @param data: i2cData_t containing data from master and answer from slave.
 * @return Positive if OK
 */
int16_t D_I2C_Read(i2cData_t *data);



#endif	/* XC_HEADER_TEMPLATE_H */

