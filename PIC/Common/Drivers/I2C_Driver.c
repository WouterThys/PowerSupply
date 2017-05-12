#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "../CommonSettings.h"
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
#define i2cAddressWrite(addr) I2C1TRN = (((addr << 1) & I2C_ADDRESS_MASK) + I2C_WRITE)
#define i2cAddressRead(addr) I2C1TRN = (((addr << 1) & I2C_ADDRESS_MASK) + I2C_READ)
#define i2cDataWrite(data) I2C1TRN = data
#define i2cDataRead() I2C1RCV

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static uint16_t readState;
static uint16_t writeState;

static bool readReady;
static bool writeReady;

static bool slaveInterrupt;
static bool masterInterrupt;

static bool firstTime;

bool I2C_ReadyToRead;


/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/

/**
 * Check if I2CSTAT is OK
 * @param mask
 * @return false if not OK
 */
static bool check(uint16_t mask);

static bool check(uint16_t mask) {
    return ((I2C1STAT & mask) > 0);
}

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/

void D_I2C_InitMaster() {
    D_I2C_Enable(false);
    
    /* Variables */
    writeState = I2C_STATE_IDLE;
    writeReady = true;
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
    _MI2C1IE = 1; // Enable
}

void D_I2C_InitSlave(uint16_t address) {
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
    readState = I2C_STATE_IDLE;
    I2C_ReadyToRead = false;
    readReady = true;
    firstTime = true;
    
    /* Interrupts master */
    _SI2C1IF = 0; // Clear flag
    _SI2C1IP = 7;
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
    writeState = I2C_STATE_IDLE;
    writeReady = true;
    firstTime = true;
    
    readState = I2C_STATE_IDLE;
    I2C_ReadyToRead = false;
    readReady = true;
    firstTime = true;
    
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

int16_t D_I2C_Write(i2cData_t *data) {
    if (!writeReady) {
        return I2C_NOK; // Still busy
    }
    writeReady = false;
    int16_t result = I2C_OK;
    while(!writeReady) {
        switch(writeState) {
            // Check the P status bit to ensure that the bus is in an Idle state.
            case I2C_STATE_IDLE: 
                if (firstTime) { // Don't check for P and S bits
                    firstTime = false;
                    writeState = I2C_STATE_START;
                } else {
                    if (!check(CHECK_S) && check(CHECK_P)) {
                        writeState = I2C_STATE_START;
                    } else {
                        result = I2C_NOK;
                        writeReady = true;
                        writeState = I2C_STATE_IDLE;
                    }
                }
                break;
                
            // Generating Start Bus Event   
            case I2C_STATE_START:
                LED1 = 0;
                I2C1CONbits.SEN = 1; // Start master event
                writeState = I2C_STATE_ADDRESS;
                break;
            // Send address and R/W bit to slave   
            case I2C_STATE_ADDRESS:
                if(masterInterrupt) {
                    masterInterrupt = false;
                    // Do checks
                    if (check(CHECK_S) && !check(CHECK_P)) {
                        i2cAddressWrite(data->address); // Address, write
                        writeState = I2C_CHECK_ADDRESS;
                    } else {
                        I2C1CONbits.PEN = 1; // Initiate stop event
                        writeState = I2C_STATE_STOP;
                        result = I2C_NOK; // ACK not received, something went wrong
                    }
                }
                break;
            // Check if address went OK
            case I2C_CHECK_ADDRESS:
                if(masterInterrupt) {
                    masterInterrupt = false;
                    // Do checks
                    if(!check(CHECK_ACKSTAT) && !check(CHECK_IWCOL)) {
                        writeState = I2C_STATE_COMMAND;
                    } else {
                        I2C1CONbits.PEN = 1; // Initiate stop event
                        writeState = I2C_STATE_STOP;
                        result = I2C_NOK; // ACK not received, something went wrong
                    }
                }
                break;
            // Write command
            case I2C_STATE_COMMAND:
                i2cDataWrite(data->command);
                writeState = I2C_CHECK_COMMAND;
                break;
            // Check if command went OK    
            case I2C_CHECK_COMMAND:
                if(masterInterrupt) {
                    masterInterrupt = false;
                    // Do checks
                    if(!check(CHECK_ACKSTAT) && !check(CHECK_IWCOL)) {
                        writeState = I2C_STATE_REPEATED_START;
                    } else {
                        I2C1CONbits.PEN = 1; // Initiate stop event
                        writeState = I2C_STATE_STOP;
                        result = I2C_NOK; // ACK not received, something went wrong
                    }
                }
                break;
            // Repeated start
            case I2C_STATE_REPEATED_START:
                I2C1CONbits.RSEN = 1;
                writeState = I2C_CHECK_REP_START;
                break;
            // Check repeated start
            case I2C_CHECK_REP_START:
                if (masterInterrupt) {
                    masterInterrupt = false;
                    i2cAddressRead(data->address);
                    writeState = I2C_STATE_ENABLE_RECEIVE;
                }
                break;
            // Enable receiving of data    
            case I2C_STATE_ENABLE_RECEIVE:
                if (masterInterrupt) {
                    masterInterrupt = false;
                    I2C1CONbits.RCEN = 1;
                    writeState = I2C_STATE_FIRST_DATA;
                }
                break;
            // Receive first 8 bits 
            case I2C_STATE_FIRST_DATA:
                if(masterInterrupt) {
                    masterInterrupt = false;
                    
                    uint8_t read = i2cDataRead();
                    data->status = (read >> 4) & I2C_STATUS_MASK;
                    data->data1 = read & I2C_FIRST_DATA_MASK;
                    
                    I2C1CONbits.ACKDT = 0; //Send ACK during Acknowledge
                    I2C1CONbits.ACKEN = 1;
                    
                    writeState = I2C_CHECK_FIRST_DATA;
                }
                break;
            // Receive enable    
            case I2C_CHECK_FIRST_DATA:
                if(masterInterrupt) {
                    masterInterrupt = false;
                    I2C1CONbits.RCEN = 1;
                    writeState = I2C_STATE_SECOND_DATA;
                }
                break;
            // Receive last 8 bits 
            case I2C_STATE_SECOND_DATA:
                if(masterInterrupt) {
                    masterInterrupt = false;

                    data->data2 = i2cDataRead();
                    
                    I2C1CONbits.ACKDT = 1; //Send NACK during Acknowledge
                    I2C1CONbits.ACKEN = 1;
                    
                    writeState = I2C_CHECK_SECOND_DATA;
                }
                break;    
            // Stop Sequence
            case I2C_CHECK_SECOND_DATA:
                if(masterInterrupt) {
                    masterInterrupt = false;
                    I2C1CONbits.PEN = 1; // Initiate stop event
                    writeState = I2C_STATE_STOP;
                }
                break;
            // End    
            case I2C_STATE_STOP:
                if(masterInterrupt) {
                    masterInterrupt = false;
                    writeState = I2C_STATE_IDLE;
                    writeReady = true;
                }
                break;
            default:
                writeState = I2C_STATE_IDLE;
                writeReady = true;
                break;    
                
        }
    }
    return result;
}

int16_t D_I2C_Read(i2cData_t *data) {
    if (!readReady) {
        return I2C_NOK;
    }
    readReady = false;
    int16_t result = I2C_OK;
    while(!readReady) {
        switch(readState) {
            // This slaves address is detected
            case I2C_STATE_IDLE:
                if (slaveInterrupt) {
                    LED1 = 0;
                    if (check(CHECK_S) && !check(CHECK_DA)) {
                        slaveInterrupt = false;
                        data->address = i2cDataRead(); // Read to clean buffer
                        readState = I2C_STATE_COMMAND;
                    } else {
                        result = I2C_NOK;
                        readReady = true;
                        readState = I2C_STATE_IDLE; 
                    }
                }
                break;
            // Receive command
            case I2C_STATE_COMMAND:
                if (slaveInterrupt) {
                    slaveInterrupt = false;
                    
                    if (check(CHECK_DA)) {
                        data->command = i2cDataRead();
                        readState = I2C_STATE_FIRST_DATA;
                    } else {
                        result = I2C_NOK;
                        readReady = true;
                        readState = I2C_STATE_IDLE; 
                    }
                }
                break;
            // Write first data byte    
            case I2C_STATE_FIRST_DATA:
                if (slaveInterrupt) { // Repeated start
                    slaveInterrupt = false;
                    
                    if (!check(CHECK_DA)) {
                        data->address = i2cDataRead(); // Read again to prevent buffer overflow
                    
                        i2cDataWrite(data->data1);
                        while (!check(CHECK_TBF)); // Wait until buffer is full
                        I2C1CONbits.SCLREL = 1; // Release clock hold

                        readState = I2C_STATE_SECOND_DATA;
                    } else {
                        result = I2C_NOK;
                        readReady = true;
                        readState = I2C_STATE_IDLE; 
                    }
                    
                }
                break;
            // Write second data
            case I2C_STATE_SECOND_DATA:
                if (slaveInterrupt) {
                    slaveInterrupt = false;
                    
                    if (!check(CHECK_BCL)) {
                        i2cDataWrite(data->data2);
                        while (!check(CHECK_TBF)); // Wait until buffer is full
                        I2C1CONbits.SCLREL = 1; // Release clock hold
                    
                        readState = I2C_CHECK_SECOND_DATA;
                    } else {
                        result = I2C_NOK;
                        readReady = true;
                        readState = I2C_STATE_IDLE; 
                    }
                }
                break;
            // Master should generate NACK and end transmission
            case I2C_CHECK_SECOND_DATA:
                if (slaveInterrupt) {
                    slaveInterrupt = false;
                    readReady = true;
                    readState = I2C_STATE_IDLE;
                }
                break;
        }
    }
    I2C_ReadyToRead = false;
    return result;
}



/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/
// I2C Master events
void __attribute__ ( (interrupt, no_auto_psv) ) _MI2C1Interrupt(void) {
    if (_MI2C1IF) {
        masterInterrupt = true;
        _MI2C1IF = 0;
    }
}

// I2C Slave events
void __attribute__ ( (interrupt, no_auto_psv) ) _SI2C1Interrupt(void) {
    if (_SI2C1IF) {
        slaveInterrupt = true;
        _SI2C1IF = 0;
        I2C_ReadyToRead = true;
    }
}

