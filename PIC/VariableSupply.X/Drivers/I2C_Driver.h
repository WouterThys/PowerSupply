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

/*******************************************************************************
 * DEFAULT SETTINGS
 ******************************************************************************/
#ifndef I2C_SLAVE
#ifndef I2C_MASTER
#define I2C_MASTER      /* Or I2C_SLAVE                                       */
#endif
#endif

#ifndef I2C_WORD_WIDE
#define I2C_WORD_WIDE   /* All data is send as 16-bit                         */
#endif
#ifndef I2C_ADDRESS
#define I2C_ADDRESS     0x55   
#endif
#ifndef IP_I2C
#define IP_I2C          5
#endif

#ifndef I2C_SCL_Dir
#define I2C_SCL_Dir     TRISBbits.TRISB6        /* SCL Direction              */
#endif
#ifndef I2C_SDA_Dir
#define I2C_SDA_Dir     TRISBbits.TRISB5        /* SDA Direction              */
#endif

#ifndef I2C_SCL_Odc
#define I2C_SCL_Odc     ODCBbits.ODCB6          /* Open drain config for SCL  */
#endif
#ifndef I2C_SDA_Odc
#define I2C_SDA_Odc     ODCBbits.ODCB5          /* Open drain config for SDA  */
#endif
/*******************************************************************************
 * DEFAULT SETTINGS
 ******************************************************************************/

#ifdef I2C2
#define I2CTRN      I2C2TRN
#define I2CRCV      I2C2RCV
#define I2CCON      I2C2CON
#define I2CCONbits  I2C2CONbits
#define I2CSTAT     I2C2STAT
#define I2CBRG      I2C2BRG
#define I2CMSK      I2C2MSK
#define I2CADD      I2C2ADD
#define MI2CIF      _MI2C2IF
#define MI2CIE      _MI2C2IE
#define MI2CIP      _MI2C2IP
#define SI2CIF      _SI2C2IF
#define SI2CIE      _SI2C2IE
#define SI2CIP      _SI2C2IP
#else
#define I2CTRN      I2C1TRN
#define I2CRCV      I2C1RCV
#define I2CCON      I2C1CON
#define I2CCONbits  I2C1CONbits
#define I2CSTAT     I2C1STAT
#define I2CBRG      I2C1BRG
#define I2CMSK      I2C1MSK
#define I2CADD      I2C1ADD
#define MI2CIF      _MI2C1IF
#define MI2CIE      _MI2C1IE
#define MI2CIP      _MI2C1IP
#define SI2CIF      _SI2C1IF
#define SI2CIE      _SI2C1IE
#define SI2CIP      _SI2C1IP
#endif

// Values
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
    uint16_t bufferAdr;
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