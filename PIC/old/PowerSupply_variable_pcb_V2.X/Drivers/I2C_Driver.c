#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>

#include "I2C_Driver.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
#define I2C_STATE_IDLE           0
#define I2C_STATE_START          1
#define I2C_STATE_ADDRESS        2
#define I2C_STATE_COMMAND        3
#define I2C_STATE_FIRST_DATA     4
#define I2C_STATE_READWRITE      5
#define I2C_STATE_SEND           6
#define I2C_STATE_SEND_ACK       7
#define I2C_STATE_READ           8
#define I2C_STATE_READ_ADDRESS   9
#define I2C_STATE_READ_ACK1      10
#define I2C_STATE_READ_RECEIVE   11
#define I2C_STATE_READ_READ      12
#define I2C_STATE_READ_ACK2      13
#define I2C_STATE_STOP           14
#define I2C_STATE_STOP_ACK       15
#define I2C_STATE_ERROR          16
#define I2C_STATE_ERROR_ACK      17
#define I2C_STATE_RETRY          18
#define I2C_STATE_RETRY_ACK      19
#define I2C_STATE_ACK_POLL       20

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

typedef struct {
    uint16_t cmd;     // Read or write command
    int16_t state;    // FSM state
    int16_t dataCnt;  // Data count
    int16_t retryCnt; // Retry count
    i2cPackage_t * data; // The data to read or write
} i2cFsm_t ;

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/
#define i2cAddressWrite(addr)   I2C1TRN = (((addr << 1) & I2C_ADDRESS_MASK) + 0)
#define i2cAddressRead(addr)    I2C1TRN = (((addr << 1) & I2C_ADDRESS_MASK) + 1)
#define i2cDataWrite(data)      I2C1TRN = data
#define i2cDataRead()           I2C1RCV
#define i2cStart()              I2C1CONbits.SEN = 1
#define i2cRepeatStart()        I2C1CONbits.RSEN = 1
#define i2cReceiveEn()          I2C1CONbits.RCEN = 1
#define i2cAckEn()              I2C1CONbits.ACKEN = 1
#define i2cAck()                I2C1CONbits.ACKDT = 0
#define i2cNAck()               I2C1CONbits.ACKDT = 1
#define i2cStop()               I2C1CONbits.PEN = 1
#define i2cSclRel()             I2C1CONbits.SCLREL = 1
#define i2cCheck(msk)           ((I2C1STAT & msk) > 0)

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
#ifdef I2C_MASTER
static volatile bool masterInterrupt;
#endif

#ifdef I2C_SLAVE
static void (*i2cEvent)(i2cPackage_t data);
static void write(i2cPackage_t * package);
static void read(i2cPackage_t * package);
#endif

static i2cFsm_t i2cFsm;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void doFsm(i2cFsm_t * );

#ifdef I2C_MASTER
void doFsm(i2cFsm_t * fsm) {   
    switch (fsm->state) {
        case I2C_STATE_IDLE: // Idle
            if ((fsm->cmd == I2C_MWRITE) || (fsm->cmd == I2C_MREAD)) {
                fsm->state = I2C_STATE_START;
            }
            break;
            
        /*==================================*/
        /* Control/Address Phase            */
        /*==================================*/
        case I2C_STATE_START: // Start condition
            i2cStart();
            fsm->state = I2C_STATE_ADDRESS;
            break;
            
        case I2C_STATE_ADDRESS: // Write address if started
            if (masterInterrupt) {
                masterInterrupt = false;
                i2cAddressWrite(fsm->data->address);
                fsm->state = I2C_STATE_COMMAND;
            }
            break;
            
        case I2C_STATE_COMMAND: // Write command
            if (masterInterrupt) {
                masterInterrupt = false;
                if (!i2cCheck(CHECK_ACKSTAT)) { // ACK
                    fsm->retryCnt = 0;
                    i2cDataWrite(fsm->data->command);
                    fsm->state = I2C_STATE_READWRITE;
                } else {
                    if (fsm->retryCnt < MAX_RETRY) {
                        fsm->state = I2C_STATE_RETRY;
                    } else {
                        fsm->data->status = I2C_NO_ADR_ACK;
                        fsm->state = I2C_STATE_ERROR; // Error and exit
                    }
                }
            }
            break;
            
        case I2C_STATE_READWRITE: // Read or write
            if (masterInterrupt) {
                masterInterrupt = false;
                if (!i2cCheck(CHECK_ACKSTAT)) { // ACK
                    switch(fsm->cmd) {
                        case I2C_MWRITE: fsm->state = I2C_STATE_SEND;  break;
                        case I2C_MREAD:  fsm->state = I2C_STATE_READ;  break;
                        default:         fsm->state = I2C_STATE_ERROR; break;
                    }
                } else {
                    fsm->data->status = I2C_NO_DATA_ACK;
                    fsm->state = I2C_STATE_ERROR; // Error and exit
                }
            }
            break;
            
            
        /*==================================*/
        /* Write Data Phase                 */
        /*==================================*/
        case I2C_STATE_SEND: // Send data
            i2cDataWrite(*(fsm->data->data + fsm->dataCnt));
            fsm->state = I2C_STATE_SEND_ACK;
            fsm->dataCnt++;
            break;
            
        case I2C_STATE_SEND_ACK: // Look for end of data or NACK
            if (masterInterrupt) {
                masterInterrupt = false;
                
                if (!i2cCheck(CHECK_ACKSTAT)) { // ACK
                    if (fsm->dataCnt >= fsm->data->length) { 
                        fsm->state = I2C_STATE_STOP; // Done
                    } else {
                        fsm->state = I2C_STATE_SEND; // Keep writing
                    }
                } else {
                    fsm->data->status = I2C_NO_DATA_ACK;
                    fsm->state = I2C_STATE_ERROR; // Error and exit
                }
            }
            break;
            
        /*==================================*/
        /* Read Data Phase                  */
        /*==================================*/
        case I2C_STATE_READ: // Repeat start
            i2cRepeatStart();
            fsm->state = I2C_STATE_READ_ADDRESS;
            break;
            
        case I2C_STATE_READ_ADDRESS: // Resend address but with read flag
            if (masterInterrupt) {
                masterInterrupt = false;
                i2cAddressRead(fsm->data->address);
                fsm->state = I2C_STATE_READ_ACK1;
            }
            break;
    
        case I2C_STATE_READ_ACK1: // Check 
            if (masterInterrupt) {
                masterInterrupt = false;
                
                if (!i2cCheck(CHECK_ACKSTAT)) { // ACK
                    fsm->state = I2C_STATE_READ_RECEIVE;
                } else {
                    fsm->data->status = I2C_NO_DATA_ACK;
                    fsm->state = I2C_STATE_ERROR; // Error and exit
                }
            }
            break;
            
        case I2C_STATE_READ_RECEIVE: // Receive enable
            i2cReceiveEn();
            fsm->state = I2C_STATE_READ_READ;
            break;
            
        case I2C_STATE_READ_READ: // Receive data
            if (masterInterrupt) {
                masterInterrupt = false;
                
                *(fsm->data->data + fsm->dataCnt) = i2cDataRead();
                fsm->dataCnt++;
                
                if (fsm->dataCnt >= fsm->data->length) {
                    i2cNAck(); // NACK to stop read
                } else {
                    i2cAck(); // ACK to keep reading
                }
                i2cAckEn();
                fsm->state = I2C_STATE_READ_ACK2;
            }
            break;
            
        case I2C_STATE_READ_ACK2:
            if (masterInterrupt) {
                masterInterrupt = false;
                
                if (fsm->dataCnt >= fsm->data->length) {
                    fsm->state = I2C_STATE_STOP;
                } else {
                    fsm->state = I2C_STATE_READ_RECEIVE;
                }
            }
            break;
            
        /*==================================*/
        /* Stop Sequence                    */
        /*==================================*/
        case I2C_STATE_STOP: // Stop condition
            i2cStop();
            fsm->state = I2C_STATE_STOP_ACK;
            break;
            
        case I2C_STATE_STOP_ACK: // Check stop
            if (masterInterrupt) {
                masterInterrupt = false;
                
                // Clear
                fsm->state = I2C_STATE_IDLE;
                fsm->dataCnt = 0;
                fsm->cmd = I2C_IDLE;
            }
            break;
            
        /*==================================*/
        /* Set Error                         */
        /*==================================*/
        case I2C_STATE_ERROR:  
            i2cStop();
            fsm->state = I2C_STATE_ERROR_ACK;
            break;
            
        case I2C_STATE_ERROR_ACK:
            if (masterInterrupt) {
                masterInterrupt = false;
                
                fsm->state = I2C_STATE_IDLE;
                fsm->retryCnt = 0;
                fsm->dataCnt = 0;
                fsm->cmd = I2C_NOK;
            }
            break;
            
        /*==================================*/
        /* Retry                             */
        /*==================================*/
        case I2C_STATE_RETRY:
            i2cStop();
            fsm->state = I2C_STATE_RETRY_ACK;
            fsm->retryCnt++;
            break;
            
        case I2C_STATE_RETRY_ACK:
            if (masterInterrupt) {
                masterInterrupt = false;
                
                fsm->state = I2C_STATE_IDLE;
                fsm->dataCnt = 0;
            }
            break;
            
        /*==================================*/
        /* ACK Polling                      */
        /*==================================*/    
            bool ackTest;
        case I2C_STATE_ACK_POLL:
            i2cStop();
            while(!_MI2C2IF);
            while(_MI2C2IF);
            ackTest = true;
            while (ackTest) {
                i2cRepeatStart();
                while(!_MI2C2IF);
                while(_MI2C2IF);
                i2cAddressWrite(fsm->data->address);
                while (_TRSTAT);
                ackTest = !i2cCheck(CHECK_ACKSTAT);
            }
            i2cStop();
            while(!_MI2C2IF);
            while(_MI2C2IF);
            masterInterrupt = false;
            fsm->state = I2C_STATE_STOP;
            break;
    }
}
#endif

#ifdef I2C_SLAVE
bool wMSB = false;
void write(i2cPackage_t * p) {
#ifdef I2C_WORD_WIDE
    uint8_t w = 0;
    if (wMSB) {                         // Write MSB
        w = (uint8_t) ((p->data[p->command]) >> 8);
        
        wMSB = false;
        p->command++;                   // Increment pointer for next read
    } else {                            // Write LSB
        w = (uint8_t) p->data[p->command];
        
        wMSB = true;                    // Don't increment command pointer
    }
    
    i2cDataWrite(w);                    // Write the value of the buffer
    i2cSclRel();                        // Release SCL line
    while (i2cCheck(CHECK_TBF));        // Wait
#else
    i2cDataWrite(p->data[p->command]);  // Write the value of the buffer
    i2cSclRel();                        // Release SCL line
    while (i2cCheck(CHECK_TBF));        // Wait
    p->command++;                       // Increment pointer for next read
#endif
}

bool rMSB = false;
void read(i2cPackage_t * p) {
#ifdef I2C_WORD_WIDE
    uint8_t r = (uint8_t) i2cDataRead();
    
    if (rMSB) {
        p->data[p->command] |= (((uint16_t)r << 8) & 0xFF00);
        rMSB = false;
        p->command++;
    } else {
        p->data[p->command] = r;
        rMSB = true;
    }
#else
    p->data[p->command] = (uint8_t) i2cDataRead();
    p->command++;
#endif
}

uint8_t i2cRead;
void doFsm(i2cFsm_t * fsm) {
    
    // Inputs
    bool mstrWrite = !i2cCheck(CHECK_RW);
    bool mstrAddr = !i2cCheck(CHECK_DA);
    i2cPackage_t * p = fsm->data;
    
    // Logic
    if (mstrAddr) {                         // Master has addressed this device
        fsm->dataCnt = 0;                   // Keep track of number of bytes
        i2cRead = i2cDataRead();            // Dummy read address     
        if (!mstrWrite) {                   // Master wants to read (repeat start)
            // Set status
            p->status = I2C_MREAD;
            
            // Handle I2C
            write(p);                       // Write the value of the buffer
            
            // Event
            (*i2cEvent)(*p);
        } else {
            p->status = I2C_MWRITE;
            
        }
    } else {                                // Master is now in data phase
        fsm->dataCnt++;                       
        if (!mstrWrite) {                   // Multiple byte read
            // Set status
            p->status = I2C_MREAD;
            
            // Handle I2C
            write(p);                       // Write the value of the buffer
            
            // Event
            (*i2cEvent)(*p);
        } else {
            if (fsm->dataCnt == 1) {        // Command byte
                // Set status
                p->status = I2C_MWRITE;
                
                // Handle I2C
                p->command = i2cDataRead(); // Set the pointer
                wMSB = false;
                rMSB = false;
            } else {                        // Read data into the buffer
                // Set status
                p->status = I2C_MWRITE;
                
                // Handle I2C
                read(p);                    // Read the value from the buffer
                
                // Event
                (*i2cEvent)(*p);
            }
        }
    }
    
//    // Safety check
//    if (p->command >= p->length) {
//        p->command = 0;
//    }
}
#endif

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/

#ifdef I2C_MASTER
void i2cDriverInit() {
    i2cDriverEnable(false);
    
    /* Variables */
    i2cFsm.state = I2C_STATE_IDLE;
    i2cFsm.cmd = I2C_IDLE;
    i2cFsm.dataCnt = 0;
    i2cFsm.retryCnt = 0;

    /* Ports */
    I2C_SCL_Odc = 0;    // Open drain
    I2C_SDA_Odc = 0;    // Open drain
    I2C_SCL_Dir = 0;
    I2C_SDA_Dir = 0;
    
    /* I2C2 Registers */
    I2C2CON = 0x0000;
    I2C2CONbits.SCLREL = 1; // Releases the SCLx clock
    I2C2STAT = 0x0000;
    I2C2BRG = 320;//180; // 100kHz clock
    
    /* Interrupts master */
    _MI2C2IF = 0; // Clear flag
    _MI2C2IP = IP_I2C; // Priority is highest
    _MI2C2IE = 1; // Enable
}
#endif

#ifdef I2C_SLAVE
void i2cDriverInit(i2cPackage_t *data, void (*onI2cEvent)(i2cPackage_t data)) {
    i2cDriverEnable(false);
    
    /* Variables */ 
    i2cEvent = onI2cEvent;
    
    data->address = I2C_ADDRESS;
    data->command = 0; // Is used as address for the data buffer in slave mode
    data->status = I2C_IDLE;
    
    i2cFsm.state = I2C_STATE_IDLE;
    i2cFsm.cmd = I2C_IDLE;
    i2cFsm.dataCnt = 0;
    i2cFsm.retryCnt = 0;
    i2cFsm.data = data;
    
    /* Ports */
    I2C_SCL_Odc = 0;    // Open drain
    I2C_SDA_Odc = 0;    // Open drain
    
    /* I2C1 Registers */
    I2C1CON = 0x0000;
    I2C1STAT = 0x0000;
    I2C1MSK = 0x0000;
    I2C1ADD = I2C_ADDRESS;

    /* Interrupts slave */
    _SI2C1IF = 0; // Clear flag
    _SI2C1IP = IP_I2C; // Priority is highest
    _SI2C1IE = 1; // Enable
}
#endif

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

#ifdef I2C_MASTER
void i2cDriverWrite(i2cPackage_t *data) { 
    
#ifdef I2C_WORD_WIDE
    
    uint16_t * old = &data->data[0];
    uint16_t tmp[data->length * 2];
    uint16_t i;
    for (i = 0; i < data->length; i++) {
        tmp[2*i] = (uint8_t) data->data[i];
        tmp[2*i+1] = (uint8_t) ((data->data[i]) >> 8);
    }
    data->length = data->length * 2;
    data->data = tmp;
    
#endif
    
    i2cFsm.data = data;
    i2cFsm.cmd = I2C_MWRITE;
    while (i2cFsm.cmd > I2C_IDLE) {
        doFsm(&i2cFsm);
    }
    
#ifdef I2C_WORD_WIDE
    data->length = data->length / 2;
    data->data = old;
#endif
    
}
#endif

#ifdef I2C_MASTER
void i2cDriverRead(i2cPackage_t * data) { 
    
#ifdef I2C_WORD_WIDE
    data->length = data->length * 2;
    
#endif
    
    i2cFsm.data = data;
    i2cFsm.cmd = I2C_MREAD;
    while (i2cFsm.cmd > I2C_IDLE) {
        doFsm(&i2cFsm);
    }
    
#ifdef I2C_WORD_WIDE
    
//    data->length = data->length / 2;
//    uint16_t i;
//    for (i = 0; i < data->length; i++) {
//        data->data[i] = 
//                ((data->data[2*i]) & 0x00FF)  
//                |
//                (((data->data[2*i+1]) << 8) & 0xFF00);
//    }
    
    uint16_t i;
    for (i = 0; i < data->length; i++) {
        printf(" - %d = %d\n", i, data->data[i]);
    }
    
#endif
    
}
#endif

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/
// I2C Master events

void __attribute__((interrupt, no_auto_psv)) _MI2C1Interrupt(void) {
    if (_MI2C1IF) {
        _MI2C1IF = 0;
#ifdef I2C_MASTER
        LED1 = !LED1;
        masterInterrupt = true;
#endif
    }
}

// I2C Slave events

void __attribute__((interrupt, no_auto_psv)) _SI2C1Interrupt(void) {
    if (_SI2C1IF) {
        _SI2C1IF = 0;
#ifdef I2C_SLAVE
      doFsm(&i2cFsm);
#endif
    }
}