#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>
#include <p33EP32MC202.h>       /* Includes true/false definition                  */

#include "../COM_Settings.h"
#include"../I2C_Settings.h"
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
static uint16_t slaveState;
static uint16_t masterState;

static bool slaveReady;
static bool masterReady;

static bool slaveInterrupt;
static bool masterInterrupt;

static bool firstTime;
static bool masterWrite;

static i2cData_t *slaveReadData;
static i2cAnswer_t *answerData;

bool I2C_ReadyToRead;
uint16_t I2C_SlaveReadResult;
uint16_t I2C_InterruptCnt;
uint16_t I2C_TestCnt;


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

static void setI2cData(i2cData_t * data);

static bool check(uint16_t mask) {
    return ((I2C1STAT & mask) > 0);
}

void setI2cData(i2cData_t * data) {
    if (((data->command >> 6) & 0x03) == COM_VAR_READ) {
        switch ((data->command) & 0x3F) {
            case 0:
                data->data1 = (answerData->value0 >> 8) & 0x00FF;
                data->data2 = (answerData->value0 >> 0) & 0x00FF;
                break;
            case 1:
                data->data1 = (answerData->value1 >> 8) & 0x00FF;
                data->data2 = (answerData->value1 >> 0) & 0x00FF;
                break;
            case 2:
                data->data1 = (answerData->value2 >> 8) & 0x00FF;
                data->data2 = (answerData->value2 >> 0) & 0x00FF;
                break;
            case 3:
                data->data1 = (answerData->value3 >> 8) & 0x00FF;
                data->data2 = (answerData->value3 >> 0) & 0x00FF;
                break;
        }
    }
}

void i2cRead() {
    switch (slaveState) {
            // This slaves address is detected
        case I2C_STATE_IDLE:
            slaveReady = false;
            I2C_SlaveReadResult = I2C_OK;

            if (check(CHECK_S) && !check(CHECK_DA)) {
                slaveReadData->address = i2cDataRead(); // Read to clean buffer
                slaveState = I2C_STATE_COMMAND;
            } else {
                I2C_SlaveReadResult = I2C_NOK;
                slaveState = I2C_STATE_STOP;
            }

            if (check(CHECK_BCL) || (check(CHECK_I2COV))) {
                I2C_SlaveReadResult = I2C_NOK;
                slaveState = I2C_STATE_STOP;
            }
            break;
            // Receive command
        case I2C_STATE_COMMAND:
            if (check(CHECK_DA)) {
                slaveReadData->command = i2cDataRead();
                setI2cData(slaveReadData);
                slaveState = I2C_STATE_FIRST_DATA;
            } else {
                I2C_SlaveReadResult = I2C_NOK;
                slaveState = I2C_STATE_STOP;
            }

            if (check(CHECK_BCL) || (check(CHECK_I2COV))) {
                I2C_SlaveReadResult = I2C_NOK;
                slaveState = I2C_STATE_STOP;
            }

            break;
            // Write/Read first data byte    
        case I2C_STATE_FIRST_DATA:
            if (!check(CHECK_DA)) { // Address, master is asking to send data
                slaveReadData->address = i2cDataRead(); // Read again to prevent buffer overflow

                i2cDataWrite(slaveReadData->data1);
                while (!check(CHECK_TBF)); // Wait until buffer is full
                I2C1CONbits.SCLREL = 1; // Release clock hold
                masterWrite = false;
                I2C_SlaveReadResult = I2C_MREAD;
            } else {
                slaveReadData->data1 = i2cDataRead(); // Read data
                masterWrite = true;
                I2C_SlaveReadResult = I2C_MWRITE;
            }
            slaveState = I2C_STATE_SECOND_DATA;

            if (check(CHECK_BCL) || (check(CHECK_I2COV))) {
                I2C_SlaveReadResult = I2C_NOK;
                slaveState = I2C_STATE_STOP;
            }
            break;
            // Write second data
        case I2C_STATE_SECOND_DATA:
            if (!masterWrite) { // Master wanted to read, send second data
                i2cDataWrite(slaveReadData->data2);
                while (!check(CHECK_TBF)); // Wait until buffer is full
                I2C1CONbits.SCLREL = 1; // Release clock hold

                slaveState = I2C_CHECK_SECOND_DATA;
            } else {
                slaveReadData->data2 = i2cDataRead(); // Read data
                slaveState = I2C_STATE_STOP;
            }
            if (check(CHECK_BCL) || (check(CHECK_I2COV))) {
                I2C_SlaveReadResult = I2C_NOK;
                slaveState = I2C_STATE_STOP;
            }

            break;
            // Master should generate NACK and end transmission
        case I2C_CHECK_SECOND_DATA:

            slaveState = I2C_STATE_STOP;

            if (check(CHECK_BCL) || (check(CHECK_I2COV))) {
                I2C_SlaveReadResult = I2C_NOK;
                slaveState = I2C_STATE_STOP;
            }

            break;
            // Clear and stop
        case I2C_STATE_STOP:
            slaveState = I2C_STATE_IDLE;
            slaveReady = true;
            if (check(CHECK_RBF) || check(CHECK_I2COV)) {
                slaveReadData->data1 = i2cDataRead();
               
                I2C1STATbits.I2COV = 0;
            }
            I2C_ReadyToRead = true;
            break;
    }
    
}

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/

void D_I2C_InitMaster() {
    D_I2C_Enable(false);

    /* Variables */
    masterState = I2C_STATE_IDLE;
    masterReady = true;
    firstTime = true;

    /* Ports */
    ODCBbits.ODCB8 = 0;
    ODCBbits.ODCB9 = 0;

    /* I2C1 Registers */
    I2C1CON = 0x0000;
    I2C1STAT = 0x0000;
    I2C1BRG = 180; // 100kHz clock

    /* Interrupts master */
    _MI2C1IF = 0; // Clear flag
    _MI2C1IP = 7; // Priority is highest
    _MI2C1IE = 1; // Enable
}

void D_I2C_InitSlave(uint16_t address, i2cData_t * slaveData, i2cAnswer_t * answer) {
    D_I2C_Enable(false);

    /* Ports */
    ODCBbits.ODCB8 = 0;
    ODCBbits.ODCB9 = 0;

    TRISBbits.TRISB8 = 1;
    TRISBbits.TRISB9 = 1;

    /* I2C1 Registers */
    I2C1CON = 0x0000;
    I2C1STAT = 0x0000;
    I2C1MSK = 0x0000;
    I2C1ADD = address;

    /* Variables */
    slaveState = I2C_STATE_IDLE;
    I2C_ReadyToRead = false;
    slaveReady = true;
    firstTime = true;
    slaveReadData = slaveData;
    answerData = answer;

    /* Interrupts master */
    _SI2C1IF = 0; // Clear flag
    _SI2C1IP = 7; // Priority is highest
    _SI2C1IE = 1; // Enable
}

void D_I2C_Enable(bool enable) {
    if (enable) {
        I2C1CONbits.I2CEN = 1; // Enable I2C
    } else {
        I2C1CONbits.I2CEN = 0; // Disable I2C
    }
}

void D_I2C_Reset() {

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
    I2C_ReadyToRead = false;
    I2C_InterruptCnt = 0;
    I2C_TestCnt = 0;

    /* Interrupts */
    _MI2C1IE = 0; // Disable
    _MI2C1IF = 0; // Clear flag
    _MI2C1IE = 1; // Enable

    _SI2C1IE = 0; // Disable
    _SI2C1IF = 0; // Clear flag
    _SI2C1IE = 1; // Enable

    /* Enable again */
    D_I2C_Enable(true);
}

int16_t D_I2C_MasterWrite(i2cData_t *data) {
    if (!masterReady) {
        return I2C_NOK; // Still busy
    }
    masterReady = false;
    int16_t result = I2C_OK;
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
                        result = I2C_NOK;
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
                    masterInterrupt = false;
                    // Do checks
                    if (check(CHECK_S) && !check(CHECK_P)) {
                        i2cAddressWrite(data->address); // Address, write
                        masterState = I2C_CHECK_ADDRESS;
                    } else {
                        I2C1CONbits.PEN = 1; // Initiate stop event
                        masterState = I2C_STATE_STOP;
                        result = I2C_NOK; // ACK not received, something went wrong
                    }
                }
                break;
                // Check if address went OK
            case I2C_CHECK_ADDRESS:
                if (masterInterrupt) {
                    masterInterrupt = false;
                    // Do checks
                    if (!check(CHECK_ACKSTAT) && !check(CHECK_IWCOL)) {
                        masterState = I2C_STATE_COMMAND;
                    } else {
                        I2C1CONbits.PEN = 1; // Initiate stop event
                        masterState = I2C_STATE_STOP;
                        result = I2C_NOK; // ACK not received, something went wrong
                    }
                }
                break;
                // Write command
            case I2C_STATE_COMMAND:
                i2cDataWrite(data->command);
                masterState = I2C_CHECK_COMMAND;
                break;
                // Check if command went OK    
            case I2C_CHECK_COMMAND:
                if (masterInterrupt) {
                    masterInterrupt = false;
                    // Do checks
                    if (!check(CHECK_ACKSTAT) && !check(CHECK_IWCOL)) {
                        masterState = I2C_STATE_FIRST_DATA;
                    } else {
                        I2C1CONbits.PEN = 1; // Initiate stop event
                        masterState = I2C_STATE_STOP;
                        result = I2C_NOK; // ACK not received, something went wrong
                    }
                }
                break;
                // Write first 8 bits 
            case I2C_STATE_FIRST_DATA:
                i2cDataWrite(data->data1);
                masterState = I2C_CHECK_FIRST_DATA;
                break;
                // Check first data  
            case I2C_CHECK_FIRST_DATA:
                if (masterInterrupt) {
                    masterInterrupt = false;
                    // Do checks
                    if (!check(CHECK_ACKSTAT) && !check(CHECK_IWCOL)) {
                        masterState = I2C_STATE_SECOND_DATA;
                    } else {
                        I2C1CONbits.PEN = 1; // Initiate stop event
                        masterState = I2C_STATE_STOP;
                        result = I2C_NOK; // ACK not received, something went wrong
                    }
                }
                break;
                // Receive last 8 bits 
            case I2C_STATE_SECOND_DATA:
                i2cDataWrite(data->data2);
                masterState = I2C_CHECK_SECOND_DATA;
                break;
                // Stop Sequence
            case I2C_CHECK_SECOND_DATA:
                if (masterInterrupt) {
                    masterInterrupt = false;
                    // Do checks
                    if (!check(CHECK_ACKSTAT) && !check(CHECK_IWCOL)) {
                        I2C1CONbits.PEN = 1; // Initiate stop event
                        masterState = I2C_STATE_STOP;
                    } else {
                        I2C1CONbits.PEN = 1; // Initiate stop event
                        masterState = I2C_STATE_STOP;
                        result = I2C_NOK; // ACK not received, something went wrong
                    }
                }
                break;
                // End    
            case I2C_STATE_STOP:
                if (masterInterrupt) {
                    masterInterrupt = false;
                    masterState = I2C_STATE_IDLE;
                    masterReady = true;
                }
                break;
            default:
                masterState = I2C_STATE_IDLE;
                masterReady = true;
                break;

        }
    }
    return result;
}

int16_t D_I2C_MasterRead(i2cData_t *data) {
    if (!masterReady) {
        return I2C_NOK; // Still busy
    }
    masterReady = false;
    int16_t result = I2C_OK;
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
                        result = I2C_NOK;
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
                    masterInterrupt = false;
                    // Do checks
                    if (check(CHECK_S) && !check(CHECK_P)) {
                        i2cAddressWrite(data->address); // Address, write
                        masterState = I2C_CHECK_ADDRESS;
                    } else {
                        I2C1CONbits.PEN = 1; // Initiate stop event
                        masterState = I2C_STATE_STOP;
                        result = I2C_NOK; // ACK not received, something went wrong
                    }
                }
                break;
                // Check if address went OK
            case I2C_CHECK_ADDRESS:
                if (masterInterrupt) {
                    masterInterrupt = false;
                    // Do checks
                    if (!check(CHECK_ACKSTAT) && !check(CHECK_IWCOL)) {
                        masterState = I2C_STATE_COMMAND;
                    } else {
                        I2C1CONbits.PEN = 1; // Initiate stop event
                        masterState = I2C_STATE_STOP;
                        result = I2C_NOK; // ACK not received, something went wrong
                    }
                }
                break;
                // Write command
            case I2C_STATE_COMMAND:
                i2cDataWrite(data->command);
                masterState = I2C_CHECK_COMMAND;
                break;
                // Check if command went OK    
            case I2C_CHECK_COMMAND:
                if (masterInterrupt) {
                    masterInterrupt = false;
                    // Do checks
                    if (!check(CHECK_ACKSTAT) && !check(CHECK_IWCOL)) {
                        masterState = I2C_STATE_REPEATED_START;
                    } else {
                        I2C1CONbits.PEN = 1; // Initiate stop event
                        masterState = I2C_STATE_STOP;
                        result = I2C_NOK; // ACK not received, something went wrong
                    }
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
                    masterInterrupt = false;
                    i2cAddressRead(data->address);
                    masterState = I2C_STATE_ENABLE_RECEIVE;
                }
                break;
                // Enable receiving of data    
            case I2C_STATE_ENABLE_RECEIVE:
                if (masterInterrupt) {
                    masterInterrupt = false;
                    I2C1CONbits.RCEN = 1;
                    masterState = I2C_STATE_FIRST_DATA;
                }
                break;
                // Receive first 8 bits 
            case I2C_STATE_FIRST_DATA:
                if (masterInterrupt) {
                    masterInterrupt = false;

                    uint8_t read = i2cDataRead();
                    data->status = (read >> 4) & I2C_STATUS_MASK;
                    data->data1 = read & I2C_FIRST_DATA_MASK;

                    I2C1CONbits.ACKDT = 0; //Send ACK during Acknowledge
                    I2C1CONbits.ACKEN = 1;

                    masterState = I2C_CHECK_FIRST_DATA;
                }
                break;
                // Receive enable    
            case I2C_CHECK_FIRST_DATA:
                if (masterInterrupt) {
                    masterInterrupt = false;
                    I2C1CONbits.RCEN = 1;
                    masterState = I2C_STATE_SECOND_DATA;
                }
                break;
                // Receive last 8 bits 
            case I2C_STATE_SECOND_DATA:
                if (masterInterrupt) {
                    masterInterrupt = false;

                    data->data2 = i2cDataRead();

                    I2C1CONbits.ACKDT = 1; //Send NACK during Acknowledge
                    I2C1CONbits.ACKEN = 1;

                    masterState = I2C_CHECK_SECOND_DATA;
                }
                break;
                // Stop Sequence
            case I2C_CHECK_SECOND_DATA:
                if (masterInterrupt) {
                    masterInterrupt = false;
                    I2C1CONbits.PEN = 1; // Initiate stop event
                    masterState = I2C_STATE_STOP;
                }
                break;
                // End    
            case I2C_STATE_STOP:
                if (masterInterrupt) {
                    masterInterrupt = false;
                    masterState = I2C_STATE_IDLE;
                    masterReady = true;
                }
                break;
            default:
                masterState = I2C_STATE_IDLE;
                masterReady = true;
                break;

        }
    }
    return result;
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
        
//        if (!I2C_ReadyToRead) {
            i2cRead();
            if (slaveState == I2C_STATE_STOP) {
                i2cRead(); // Execute one more time
            }
//        }
        
    }
}

