#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>

#include "../../Common/COM_Settings.h"  /* Common communication variables     */
#include "../Settings.h"
#include "../utils.h"
#include "I2C_Driver.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
#define I2C_STATE_IDLE           0
#define I2C_STATE_START          1
#define I2C_STATE_ADDRESS        2
#define I2C_STATE_COMMAND        3
#define I2C_STATE_FIRST_DATA     4
#define I2C_STATE_SECOND_DATA    5
#define I2C_STATE_STOP           6
#define I2C_STATE_ENABLE_RECEIVE 7
#define I2C_STATE_REPEATED_START 8

#define I2C_CHECK_ADDRESS        10
#define I2C_CHECK_COMMAND        11
#define I2C_CHECK_FIRST_DATA     12
#define I2C_CHECK_SECOND_DATA    13
#define I2C_CHECK_REP_START      14

#define I2C_ADDRESS_MASK    0xFE    /* Mask for fist 7 bits in address byte   */
#define I2C_RW_MASK         0x01    /* Mask for write or read indicator       */

#define CHECK_ACKSTAT 0x8000 /* Mask for ACKSTAT. ACK = 0, NACK = 1           */
#define CHECK_BCL     0x0400 /* Mask for Bus Collision Detect                 */
#define CHECK_IWCOL   0x0080 /* Mask for Write Collision Detect               */
#define CHECK_I2COV   0x0040 /* Mask for Receive overflow Detect              */
#define CHECK_DA      0x0020 /* Mask for Data/nAddress (Slave mode). 1 = data */
#define CHECK_P       0x0010 /* Mask for Stop bit. 1 = Stop bit detected last */
#define CHECK_S       0x0008 /* Mask for Start bit. 1 = Start bit detected last*/
#define CHECK_RW      0x0004 /* Mask for Read/Write (Slave mode). 1 = Read    */
#define CHECK_RBF     0x0002 /* Mask for Receive Buffer Full                  */
#define CHECK_TBF     0x0001 /* Mask for Transmit Buffer Full                 */

#define MAX_TIMEOUT   20

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/
#define i2cAddressWrite(addr) I2C1TRN = (((addr << 1) & I2C_ADDRESS_MASK) + 0)
#define i2cAddressRead(addr) I2C1TRN = (((addr << 1) & I2C_ADDRESS_MASK) + 1)
#define i2cDataWrite(data) I2C1TRN = data
#define i2cDataRead() I2C1RCV

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static volatile uint16_t slaveState;
static volatile uint16_t masterState;

static bool slaveReady;
static bool masterReady;

static bool slaveInterrupt;
static bool masterInterrupt;

static bool firstTime;
static bool masterWrite;

static i2cPackage_t slaveReadData;

static uint16_t timeoutCnt;
static uint16_t dataCnt;


/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/

/**
 * Check if I2CSTAT is OK
 * @param mask
 * @return false if not OK
 */
static bool check(uint16_t mask);
static void i2cRead();

static void (*onI2cAnswer)(i2cPackage_t  * data);
static void (*onI2cReadDone)(i2cPackage_t data);

static bool check(uint16_t mask) {
    return ((I2C1STAT & mask) > 0);
}


uint8_t read;
void i2cRead() {
    switch (slaveState) {
            // This slaves address is detected
        case I2C_STATE_IDLE:
            slaveReady = false;
            slaveReadData.status = I2C_OK;
            dataCnt = 0;
            masterWrite = true;

            if (check(CHECK_S) && !check(CHECK_DA)) {
                slaveReadData.address = i2cDataRead(); // Read to clean buffer
                slaveState = I2C_STATE_COMMAND;
            } else {
                slaveReadData.status = I2C_UNEXPECTED_DATA;
                slaveState = I2C_STATE_STOP;
            }
            // Error checks
            if (check(CHECK_BCL)) {
                slaveReadData.status = I2C_COLLISION;
                slaveState = I2C_STATE_STOP;
            }
            if (check(CHECK_I2COV)) {
                slaveReadData.status = I2C_OVERFLOW;
                slaveState = I2C_STATE_STOP;
            }
            break;
            // Receive command
        case I2C_STATE_COMMAND:
            if (check(CHECK_DA)) {
                read =  i2cDataRead();
                slaveReadData.command = (read & 0x0F);
                slaveReadData.length = ((read >> 4) & 0x0F);
                slaveState = I2C_STATE_FIRST_DATA;
            } else {
                slaveReadData.status = I2C_UNEXPECTED_ADR;
                slaveState = I2C_STATE_STOP;
            }

            // Error checks
            if (check(CHECK_BCL)) {
                slaveReadData.status = I2C_COLLISION;
                slaveState = I2C_STATE_STOP;
            }
            if (check(CHECK_I2COV)) {
                slaveReadData.status = I2C_OVERFLOW;
                slaveState = I2C_STATE_STOP;
            }

            break;
            
            
            
            
            // Write/Read first data byte    
        case I2C_STATE_FIRST_DATA:
            if (!check(CHECK_DA)) { // Address, master is asking to send data
                slaveReadData.address = i2cDataRead(); // Read again to prevent buffer overflow
                (*onI2cAnswer)(&slaveReadData);
                i2cDataWrite(slaveReadData.data[dataCnt].data1);
                while (!check(CHECK_TBF)); // Wait until buffer is full
                I2C1CONbits.SCLREL = 1; // Release clock hold
                masterWrite = false;
                slaveReadData.status = I2C_MREAD;
            } else {
                slaveReadData.data[dataCnt].data1 = i2cDataRead(); // Read data
                masterWrite = true;
                slaveReadData.status = I2C_MWRITE;
            }
            slaveState = I2C_STATE_SECOND_DATA;

            // Error checks
            if (check(CHECK_BCL)) {
                slaveReadData.status = I2C_COLLISION;
                slaveState = I2C_STATE_STOP;
            }
            if (check(CHECK_I2COV)) {
                slaveReadData.status = I2C_OVERFLOW;
                slaveState = I2C_STATE_STOP;
            }
            break;
            // Write second data
        case I2C_STATE_SECOND_DATA:
            dataCnt++;
            if (!masterWrite) { // Master wanted to read, send second data
                i2cDataWrite(slaveReadData.data[dataCnt].data1);
                while (!check(CHECK_TBF)); // Wait until buffer is full
                I2C1CONbits.SCLREL = 1; // Release clock hold
                if (dataCnt < slaveReadData.length) {
                    slaveState = I2C_STATE_FIRST_DATA;
                } else {
                    slaveState = I2C_CHECK_SECOND_DATA;
                }
            } else {
                slaveReadData.data[dataCnt].data2 = i2cDataRead(); // Read data
                if (dataCnt < slaveReadData.length) {
                    slaveState = I2C_STATE_FIRST_DATA;
                } else {
                    slaveState = I2C_STATE_STOP;
                }
            }
            // Error checks
            if (check(CHECK_BCL)) {
                slaveReadData.status = I2C_COLLISION;
                slaveState = I2C_STATE_STOP;
            }
            if (check(CHECK_I2COV)) {
                slaveReadData.status = I2C_OVERFLOW;
                slaveState = I2C_STATE_STOP;
            }

            break;
            // Master should generate NACK and end transmission
        case I2C_CHECK_SECOND_DATA:
           slaveState = I2C_STATE_STOP;
            // Error checks
            if (check(CHECK_BCL)) {
                slaveReadData.status = I2C_COLLISION;
                slaveState = I2C_STATE_STOP;
            }
            if (check(CHECK_I2COV)) {
                slaveReadData.status = I2C_OVERFLOW;
                slaveState = I2C_STATE_STOP;
            }
            break;
            
            
            
            
            
            // Clear and stop
        case I2C_STATE_STOP:
            slaveState = I2C_STATE_IDLE;
            slaveReady = true;
            if (check(CHECK_RBF) || check(CHECK_I2COV)) {
                slaveReadData.data[0].data1 = i2cDataRead();

                I2C1STATbits.I2COV = 0;
            }
            (*onI2cReadDone)(slaveReadData);
            break;
    }

}

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/

void i2cDriverInitMaster() {
    i2cDriverEnable(false);

    /* Variables */
    masterState = I2C_STATE_IDLE;
    masterReady = true;
    firstTime = true;
    timeoutCnt = 0;
    dataCnt = 0;

    /* Ports */
    I2C_SCL_Odc = 0;    // Open drain
    I2C_SDA_Odc = 0;    // Open drain
    I2C_SCL_Dir = 0;    // Output
    I2C_SDA_Dir = 0;    // Output
    
    /* I2C1 Registers */
    I2C1CON = 0x0000;
    I2C1STAT = 0x0000;
    I2C1BRG = 180;//180; // 100kHz clock

    /* Interrupts master */
    _MI2C1IF = 0; // Clear flag
    _MI2C1IP = IP_I2C; // Priority is highest
    _MI2C1IE = 1; // Enable
}

void i2cDriverInitSlave(uint16_t address, void (*answer)(i2cPackage_t * data), void (*readDone)(i2cPackage_t data)) {
    i2cDriverEnable(false);
    
    /* Ports */
    I2C_SCL_Odc = 0;    // Open drain
    I2C_SDA_Odc = 0;    // Open drain
    I2C_SCL_Dir = 1;    // Input
    I2C_SDA_Dir = 1;    // Input

    /* I2C1 Registers */
    I2C1CON = 0x0000;
    I2C1STAT = 0x0000;
    I2C1MSK = 0x0000;
    I2C1ADD = address;

    /* Variables */
    slaveState = I2C_STATE_IDLE;
    slaveReady = true;
    firstTime = true;
    
    /* Event function pointers */
    onI2cAnswer = answer;
    onI2cReadDone = readDone;

    /* Interrupts master */
    _SI2C1IF = 0; // Clear flag
    _SI2C1IP = IP_I2C; // Priority is highest
    _SI2C1IE = 1; // Enable
}

void i2cDriverEnable(bool enable) {
    if (enable) {
        I2C1CONbits.I2CEN = 1; // Enable I2C
    } else {
        I2C1CONbits.I2CEN = 0; // Disable I2C
    }
}

void i2cDriverReset() {

    /* I2C1 Registers */
    I2C1CON = 0x0000; // Disables module
    I2C1STAT = 0x0000;

    /* Variables */
    masterState = I2C_STATE_IDLE;
    masterReady = true;
    firstTime = true;

    slaveState = I2C_STATE_IDLE;
    slaveReady = true;
    firstTime = true;
    dataCnt = 0;

    /* Interrupts */
    _MI2C1IE = 0; // Disable
    _MI2C1IF = 0; // Clear flag
    _MI2C1IE = 1; // Enable

    _SI2C1IE = 0; // Disable
    _SI2C1IF = 0; // Clear flag
    _SI2C1IE = 1; // Enable

    /* Enable again */
    i2cDriverEnable(true);
}

void i2cDriverMasterWrite(i2cPackage_t *data) {
    if (!masterReady) {
        data->status = I2C_STILL_BUSY; // Still busy
        return;
    }
    uint8_t command = ((data->length << 4) & 0xF0) | (data->command & 0x0F);
    masterReady = false;
    masterInterrupt = false;
    timeoutCnt = 0;
    dataCnt = 0;
    data->status = I2C_OK;
    while (!masterReady) {
        switch (masterState) {
                // Check the P status bit to ensure that the bus is in an Idle state.
            case I2C_STATE_IDLE:
                if (firstTime) { // Don't check for P and S bits
                    firstTime = false;
                    masterState = I2C_STATE_START;
                } else {
                    if (!check(CHECK_S) && check(CHECK_P)) {
                        masterState = I2C_STATE_START;
                    } else {
                        data->status = I2C_NOK;
                        masterReady = true;
                        masterState = I2C_STATE_IDLE;
                    }
                }
                break;

                // Generating Start Bus Event   
            case I2C_STATE_START:
                I2C1CONbits.SEN = 1; // Start master event
                masterState = I2C_STATE_ADDRESS;
                break;
                // Send address and R/W bit to slave   
            case I2C_STATE_ADDRESS:
                if (masterInterrupt) {
                    timeoutCnt = 0;
                    masterInterrupt = false;
                    // Do checks
                    if (check(CHECK_S) && !check(CHECK_P)) {
                        i2cAddressWrite(data->address); // Address, write
                        masterState = I2C_CHECK_ADDRESS;
                    } else {
                        I2C1CONbits.PEN = 1; // Initiate stop event
                        masterState = I2C_STATE_STOP;
                        data->status = I2C_NOK; // ACK not received, something went wrong
                    }
                } else {
//                    timeoutCnt++;
//                    if (timeoutCnt > MAX_TIMEOUT) {
//                        I2C1CONbits.PEN = 1; // Initiate stop event
//                        masterState = I2C_STATE_STOP;
//                        data->status = I2C_TIMEOUT; // Too long before interrupt came
//                    }
                }
                break;
                // Check if address went OK
            case I2C_CHECK_ADDRESS:
                if (masterInterrupt) {
                    timeoutCnt = 0;
                    masterInterrupt = false;
                    // Do checks
                    if (!check(CHECK_ACKSTAT) && !check(CHECK_IWCOL)) {
                        masterState = I2C_STATE_COMMAND;
                    } else {
                        I2C1CONbits.PEN = 1; // Initiate stop event
                        masterState = I2C_STATE_STOP;
                        data->status = I2C_NO_ADR_ACK; // ACK not received, something went wrong
                    }
                } else {
//                    timeoutCnt++;
//                    if (timeoutCnt > MAX_TIMEOUT) {
//                        I2C1CONbits.PEN = 1; // Initiate stop event
//                        masterState = I2C_STATE_STOP;
//                        data->status = I2C_TIMEOUT; // Too long before interrupt came
//                    }
                }
                break;
                // Write command
            case I2C_STATE_COMMAND:
                i2cDataWrite(command);
                masterState = I2C_CHECK_COMMAND;
                break;
                // Check if command went OK    
            case I2C_CHECK_COMMAND:
                if (masterInterrupt) {
                    timeoutCnt = 0;
                    masterInterrupt = false;
                    // Do checks
                    if (!check(CHECK_ACKSTAT) && !check(CHECK_IWCOL)) {
                        masterState = I2C_STATE_FIRST_DATA;
                    } else {
                        I2C1CONbits.PEN = 1; // Initiate stop event
                        masterState = I2C_STATE_STOP;
                        data->status = I2C_NO_DATA_ACK; // ACK not received, something went wrong
                    }
                } else {
//                    timeoutCnt++;
//                    if (timeoutCnt > MAX_TIMEOUT) {
//                        I2C1CONbits.PEN = 1; // Initiate stop event
//                        masterState = I2C_STATE_STOP;
//                        data->status = I2C_TIMEOUT; // Too long before interrupt came
//                    }
                }
                break;
                
                
                
                
                
                // Write first 8 bits 
            case I2C_STATE_FIRST_DATA:
                i2cDataWrite(data->data[dataCnt].data1);
                masterState = I2C_CHECK_FIRST_DATA;
                break;
                // Check first data  
            case I2C_CHECK_FIRST_DATA:
                if (masterInterrupt) {
                    timeoutCnt = 0;
                    masterInterrupt = false;
                    // Do checks
                    if (!check(CHECK_ACKSTAT) && !check(CHECK_IWCOL)) {
                        masterState = I2C_STATE_SECOND_DATA;
                    } else {
                        I2C1CONbits.PEN = 1; // Initiate stop event
                        masterState = I2C_STATE_STOP;
                        data->status = I2C_NO_DATA_ACK; // ACK not received, something went wrong
                    }
                } else {
//                    timeoutCnt++;
//                    if (timeoutCnt > MAX_TIMEOUT) {
//                        I2C1CONbits.PEN = 1; // Initiate stop event
//                        masterState = I2C_STATE_STOP;
//                        data->status = I2C_TIMEOUT; // Too long before interrupt came
//                    }
                }
                break;
                // Receive last 8 bits 
            case I2C_STATE_SECOND_DATA:
                i2cDataWrite(data->data[dataCnt].data2);
                masterState = I2C_CHECK_SECOND_DATA;
                break;
                // Stop Sequence
            case I2C_CHECK_SECOND_DATA:
                if (masterInterrupt) {
                    timeoutCnt = 0;
                    masterInterrupt = false;
                    // Do checks
                    if (!check(CHECK_ACKSTAT) && !check(CHECK_IWCOL)) {
                        dataCnt++;
                        if (dataCnt < data->length) {
                            masterState = I2C_STATE_FIRST_DATA;
                        } else {
                            I2C1CONbits.PEN = 1; // Initiate stop event
                            masterState = I2C_STATE_STOP;
                        }
                    } else {
                        I2C1CONbits.PEN = 1; // Initiate stop event
                        masterState = I2C_STATE_STOP;
                        data->status = I2C_NO_DATA_ACK; // ACK not received, something went wrong
                    }
                } else {
//                    timeoutCnt++;
//                    if (timeoutCnt > MAX_TIMEOUT) {
//                        I2C1CONbits.PEN = 1; // Initiate stop event
//                        masterState = I2C_STATE_STOP;
//                        data->status = I2C_TIMEOUT; // Too long before interrupt came
//                    }
                }
                break;
                
                
                
                
                
                // End    
            case I2C_STATE_STOP:
                if (masterInterrupt) {
                    masterInterrupt = false;
                    masterState = I2C_STATE_IDLE;
                    masterReady = true;
                } else {
//                    timeoutCnt++;
//                    if (timeoutCnt > MAX_TIMEOUT) {
//                        masterInterrupt = false;
//                        masterState = I2C_STATE_IDLE;
//                        masterReady = true;
//                    }
                }
                break;
            default:
                masterState = I2C_STATE_IDLE;
                masterReady = true;
                break;

        }
    }
}

void i2cDriverMasterRead(i2cPackage_t *data) {
    if (!masterReady) {
        data->status = I2C_STILL_BUSY; // Still busy
        return;
    }
    uint8_t command = ((data->length << 4) & 0xF0) | (data->command & 0x0F);
    masterReady = false;
    masterInterrupt = false;
    timeoutCnt = 0;
    dataCnt = 0;
    data->status = I2C_OK;
    while (!masterReady) {

        switch (masterState) {
                // Check the P status bit to ensure that the bus is in an Idle state.
            case I2C_STATE_IDLE:
                if (firstTime) { // Don't check for P and S bits
                    firstTime = false;
                    masterState = I2C_STATE_START;
                } else {
                    if (!check(CHECK_S) && check(CHECK_P)) {
                        masterState = I2C_STATE_START;
                    } else {
                        data->status = I2C_NOK;
                        masterReady = true;
                        masterState = I2C_STATE_IDLE;
                    }
                }
                break;

                // Generating Start Bus Event   
            case I2C_STATE_START:
                I2C1CONbits.SEN = 1; // Start master event
                masterState = I2C_STATE_ADDRESS;
                break;
                // Send address and R/W bit to slave   
            case I2C_STATE_ADDRESS:
                if (masterInterrupt) {
                    timeoutCnt = 0;
                    masterInterrupt = false;
                    // Do checks
                    if (check(CHECK_S) && !check(CHECK_P)) {
                        i2cAddressWrite(data->address); // Address, write
                        masterState = I2C_CHECK_ADDRESS;
                    } else {
                        I2C1CONbits.PEN = 1; // Initiate stop event
                        masterState = I2C_STATE_STOP;
                        data->status = I2C_NO_ADR_ACK; // ACK not received, something went wrong
                    }
                } else {
//                    timeoutCnt++;
//                    if (timeoutCnt > MAX_TIMEOUT) {
//                        I2C1CONbits.PEN = 1; // Initiate stop event
//                        masterState = I2C_STATE_STOP;
//                        data->status = I2C_TIMEOUT; // Too long before interrupt came
//                    }
                }
                break;
                // Check if address went OK
            case I2C_CHECK_ADDRESS:
                if (masterInterrupt) {
                    timeoutCnt = 0;
                    masterInterrupt = false;
                    // Do checks
                    if (!check(CHECK_ACKSTAT) && !check(CHECK_IWCOL)) {
                        masterState = I2C_STATE_COMMAND;
                    } else {
                        I2C1CONbits.PEN = 1; // Initiate stop event
                        masterState = I2C_STATE_STOP;
                        data->status = I2C_NO_ADR_ACK; // ACK not received, something went wrong
                    }
                } else {
//                    timeoutCnt++;
//                    if (timeoutCnt > MAX_TIMEOUT) {
//                        I2C1CONbits.PEN = 1; // Initiate stop event
//                        masterState = I2C_STATE_STOP;
//                        data->status = I2C_TIMEOUT; // Too long before interrupt came
//                    }
                }
                break;
                // Write command
            case I2C_STATE_COMMAND:
                i2cDataWrite(command);
                masterState = I2C_CHECK_COMMAND;
                break;
                // Check if command went OK    
            case I2C_CHECK_COMMAND:
                if (masterInterrupt) {
                    timeoutCnt = 0;
                    masterInterrupt = false;
                    // Do checks
                    if (!check(CHECK_ACKSTAT) && !check(CHECK_IWCOL)) {
                        masterState = I2C_STATE_REPEATED_START;
                    } else {
                        I2C1CONbits.PEN = 1; // Initiate stop event
                        masterState = I2C_STATE_STOP;
                        data->status = I2C_NO_DATA_ACK; // ACK not received, something went wrong
                    }
                } else {
//                    timeoutCnt++;
//                    if (timeoutCnt > MAX_TIMEOUT) {
//                        I2C1CONbits.PEN = 1; // Initiate stop event
//                        masterState = I2C_STATE_STOP;
//                        data->status = I2C_TIMEOUT; // Too long before interrupt came
//                    }
                }
                break;
                // Repeated start
            case I2C_STATE_REPEATED_START:
                I2C1CONbits.RSEN = 1;
                masterState = I2C_CHECK_REP_START;
                break;
                // Check repeated start
            case I2C_CHECK_REP_START:
                if (masterInterrupt) {
                    timeoutCnt = 0;
                    masterInterrupt = false;
                    i2cAddressRead(data->address);
                    masterState = I2C_STATE_ENABLE_RECEIVE;
                } else {
//                    timeoutCnt++;
//                    if (timeoutCnt > MAX_TIMEOUT) {
//                        I2C1CONbits.PEN = 1; // Initiate stop event
//                        masterState = I2C_STATE_STOP;
//                        data->status = I2C_TIMEOUT; // Too long before interrupt came
//                    }
                }
                break;
                // Enable receiving of data    
            case I2C_STATE_ENABLE_RECEIVE:
                if (masterInterrupt) {
                    timeoutCnt = 0;
                    masterInterrupt = false;
                    I2C1CONbits.RCEN = 1;
                    masterState = I2C_STATE_FIRST_DATA;
                } else {
//                    timeoutCnt++;
//                    if (timeoutCnt > MAX_TIMEOUT) {
//                        I2C1CONbits.PEN = 1; // Initiate stop event
//                        masterState = I2C_STATE_STOP;
//                        data->status = I2C_TIMEOUT; // Too long before interrupt came
//                    }
                }
                break;
                
                
                
                
                // Receive first 8 bits 
            case I2C_STATE_FIRST_DATA:
                if (masterInterrupt) {
                    timeoutCnt = 0;
                    masterInterrupt = false;

                    data->data[dataCnt].data1 = i2cDataRead();

                    I2C1CONbits.ACKDT = 0; //Send ACK during Acknowledge
                    I2C1CONbits.ACKEN = 1;

                    masterState = I2C_CHECK_FIRST_DATA;
                } else {
//                    timeoutCnt++;
//                    if (timeoutCnt > MAX_TIMEOUT) {
//                        I2C1CONbits.PEN = 1; // Initiate stop event
//                        masterState = I2C_STATE_STOP;
//                        data->status = I2C_TIMEOUT; // Too long before interrupt came
//                    }
                }
                break;
                // Receive enable    
            case I2C_CHECK_FIRST_DATA:
                if (masterInterrupt) {
                    timeoutCnt = 0;
                    masterInterrupt = false;
                    I2C1CONbits.RCEN = 1;
                    masterState = I2C_STATE_SECOND_DATA;
                } else {
//                    timeoutCnt++;
//                    if (timeoutCnt > MAX_TIMEOUT) {
//                        I2C1CONbits.PEN = 1; // Initiate stop event
//                        masterState = I2C_STATE_STOP;
//                        data->status = I2C_TIMEOUT; // Too long before interrupt came
//                    }
                }
                break;
                // Receive last 8 bits 
            case I2C_STATE_SECOND_DATA:
                if (masterInterrupt) {
                    timeoutCnt = 0;
                    masterInterrupt = false;

                    data->data[dataCnt].data2 = i2cDataRead();
                    
                    dataCnt++;
                    if (dataCnt < data->length) {
                        I2C1CONbits.ACKDT = 1; //Send NACK during Acknowledge
                    } else {
                        I2C1CONbits.ACKDT = 0; //Send ACK during Acknowledge
                    }
                    I2C1CONbits.ACKEN = 1;

                    masterState = I2C_CHECK_SECOND_DATA;
                } else {
//                    timeoutCnt++;
//                    if (timeoutCnt > MAX_TIMEOUT) {
//                        I2C1CONbits.PEN = 1; // Initiate stop event
//                        masterState = I2C_STATE_STOP;
//                        data->status = I2C_TIMEOUT; // Too long before interrupt came
//                    }
                }
                break;
                // Stop Sequence
            case I2C_CHECK_SECOND_DATA:
                if (masterInterrupt) {
                    timeoutCnt = 0;
                    masterInterrupt = false;
                    
                    if (dataCnt < data->length) {
                        I2C1CONbits.RCEN = 1;
                        masterState = I2C_STATE_FIRST_DATA;
                    } else {
                        I2C1CONbits.PEN = 1; // Initiate stop event
                        masterState = I2C_STATE_STOP;
                    }
                } else {
//                    timeoutCnt++;
//                    if (timeoutCnt > MAX_TIMEOUT) {
//                        I2C1CONbits.PEN = 1; // Initiate stop event
//                        masterState = I2C_STATE_STOP;
//                        data->status = I2C_TIMEOUT; // Too long before interrupt came
//                    }
                }
                break;
                // End    
            case I2C_STATE_STOP:
                if (masterInterrupt) {
                    masterInterrupt = false;
                    masterState = I2C_STATE_IDLE;
                    masterReady = true;
                } else {
//                    timeoutCnt++;
//                    if (timeoutCnt > MAX_TIMEOUT) {
//                        masterInterrupt = false;
//                        masterState = I2C_STATE_IDLE;
//                        masterReady = true;
//                    }
                }
                break;
            default:
                masterState = I2C_STATE_IDLE;
                masterReady = true;
                break;

        }
    }
}

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/
// I2C Master events

void __attribute__((interrupt, no_auto_psv)) _MI2C1Interrupt(void) {
    if (_MI2C1IF) {
        masterInterrupt = true;
        _MI2C1IF = 0;
    }
}

// I2C Slave events

void __attribute__((interrupt, no_auto_psv)) _SI2C1Interrupt(void) {
    if (_SI2C1IF) {
        slaveInterrupt = true;
        _SI2C1IF = 0;

        i2cRead();
        if (slaveState == I2C_STATE_STOP) {
            i2cRead(); // Execute one more time
        }
    }
}
