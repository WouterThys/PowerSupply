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

#define I2C_ADDRESS_MASK    0xFE    /* Mask for fist 7 bits in address byte   */
#define I2C_RW_MASK         0x01    /* Mask for write or read indicator       */

#define I2C_STATUS_MASK     0x0F    /* Mask for 4-bit status                  */
#define I2C_FIRST_DATA_MASK 0x0F    /* Mask for 4-bit first data              */
/*

 */

extern bool I2C_ReadyToRead; /* Flag to indicate a read was performed         */
extern uint16_t I2C_SlaveReadResult;  /* Result of the read operation         */

struct I2C_Data {
    uint8_t address;    /* Address (7-bit) of the slave                       */
    uint8_t command;    /* Command (8-bit) send from M -> S                   */
    uint8_t status;     /* Status (4-bit) send from S -> M                    */
    uint8_t data1;      /* First data (8-bit) send from S -> M                */
    uint8_t data2;      /* Second data (8-bit) send from S -> M               */
};
typedef struct I2C_Data i2cData_t;

struct I2C_AnswerData {
    int16_t value0;
    int16_t value1;
    int16_t value2;
    int16_t value3;
};
typedef struct I2C_AnswerData i2cAnswer_t;

/**
 * 
 */
void D_I2C_InitMaster();

/**
 * 
 * @param address
 * @param slaveData
 * @param answerData
 */
void D_I2C_InitSlave(uint16_t address, i2cData_t * slaveData, i2cAnswer_t * answerData);

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
int16_t D_I2C_MasterWrite(i2cData_t *data);

/**
 * Read data from slave
 * @param data
 * @return Positive if OK
 */
int16_t D_I2C_MasterRead(i2cData_t *data);

/**
 * Read data from master, after the Master has initiated a transaction.
 * @param data: i2cData_t containing data from master and answer from slave.
 * @return Positive if OK
 */
int16_t D_I2C_SlaveRead(i2cData_t *data);



#endif	/* XC_HEADER_TEMPLATE_H */

