#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <string.h>
#include <p33EP256GP502.h>

#include "utils.h"
#include "Settings.h"

#include "Drivers/SYSTEM_Driver.h"
#include "Drivers/UART_Driver.h"
#include "Drivers/I2C_Driver.h"
#include "Drivers/ADC_Driver.h"

#include "Controllers/DAC_Controller.h"
#include "Controllers/PID_Controller.h"


/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
#define I2C_COM_SET_V   0
#define I2C_COM_SET_I   1
#define I2C_COM_MSR_V   2
#define I2C_COM_MSR_I   3
#define I2C_COM_MSR_T   4

#define PID_TEST_CNT        128
#define TEST_VOLTAGE_STEP   200
#define TEST_BUFFER_CNT     50

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/**
 * From voltage v [mV] to DAC voltage for setting output voltage.
 * Vout = Vdac * Vgain
 * @param v: the desired voltage at the output 
 */
#define toDACvoltage(v)         (((float)v) / (Vgain * 1000))

/**
 * From current i [mA] to DAC voltage for setting maximum current.
 * Imax = Vdac / (Rs * Igain)
 * @param i: the desired maximum current
 */
#define toDACcurrent(i)         (((float)i * Rs * Igain) / (1000))

/**
 * From digital value v, measured as output voltage, to voltage [mV]
 * @param v: digital value from ADC 
 */
#define fromADCtoVoltage(v)     ((adcValueToVoltage(v) * Vgain) * 1000)

/**
 * From digital value v, measured as input current, to current [mA]
 * Imax = Vdac / (Rs * Igain)
 * @param v: digital value from ADC
 */
#define fromADCtoCurrent(v)     (((adcValueToVoltage(v) / (Rs * Igain)) * 1000))

/**
 * From digital value v, measured as temperature, to temperature [°C]
 * Vt = Rt * T * 10E-6
 */
#define fromADCtoTemperature(v) ( (adcValueToVoltage(v)) / ((float)Rt * 10E-6))


/*******************************************************************************
 *          DEFINES
 ******************************************************************************/



/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static uint16_t dataArray[5];
static uint16_t * setVoltage;
static uint16_t * setCurrent;
static uint16_t * msrVoltage;
static uint16_t * msrCurrent;
static uint16_t * msrTemperature;

static i2cPackage_t i2cPackage;
static int16_t i2cError;

static bool i2cDone = false;
static bool adcDone = false;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize();
static void clipEnable(bool enable);

static void ledTimerEnable(bool enable);
static bool checkI2cState(i2cPackage_t data);

static void onI2cDone(i2cPackage_t data);
static void onAdcReadDone(uint16_t buffer, uint16_t * data);
static void onUartReadDone(UartData_t data);

void initialize() {
    sysInterruptEnable(false);
    
    // Initialize system
    sysInitPll();
    sysInitOscillator();
    sysInitPorts();
    
    // Interrupts
    sysInitInterrupts();
    sysInterruptEnable(true);
}

void clipEnable(bool enable) {
    if (enable) {
        CLIP_PIN_Dir = 1; // Input
        CLIP_PIN_Cn = 1; // Enable change notification

        _CNIF = 0; // Clear flag
        _CNIP = IP_CN; // Priority
        _CNIE = 1; // Enable interrupt
    } else {
        CLIP_PIN_Dir = 0; // Output
        CLIP_PIN_Cn = 0; // Disable change notification
        _CNIE = 0; // Disable interrupt
    }
}

void ledTimerEnable(bool enable) {
    T2CONbits.TON = 0; // Disable
    T2CONbits.TCS = 0; // Internal clock (Fp)
    T2CONbits.T32 = 0; // 16-bit timer
    T2CONbits.TCKPS = 0b11; // 1:256
    
    // Registers
    TMR2 = 0x0000;
    PR2 = 0xFFFF;
    
    // Interrupts
    _T2IP = 1;
    _T2IF = 0; // Clear
    _T2IE = 1; // Enable
    
    if (enable) {
        T2CONbits.TON = 1; // Enable
    } 
}

bool checkI2cState(i2cPackage_t data) {
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
                //case I2C_TIMEOUT: printf("I2C_TIMEOUT\n"); break;
            }
        }
    //}
    
    if (i2cError < I2C_OK) {
        //i2cDriverReset();
        return false;
    } else {
        return true; 
    }
}

void onUartReadDone(UartData_t data) {
    
}



/*******************************************************************************
 *          MAIN PROGRAM
 ******************************************************************************/

int main(void) {
    initialize();
    
    // Variables
    setVoltage = &dataArray[I2C_COM_SET_V];
    setCurrent = &dataArray[I2C_COM_SET_I];
    msrVoltage = &dataArray[I2C_COM_MSR_V];
    msrCurrent = &dataArray[I2C_COM_MSR_I];
    msrTemperature = &dataArray[I2C_COM_MSR_T];
    
    *msrVoltage = 0x0001;
    *msrCurrent = 0x0002;
    *msrTemperature = 0x0003;
    
    // Initial values
    i2cPackage.address = I2C_ADDRESS;
    i2cPackage.data = dataArray;
    i2cPackage.length = 5;
    i2cError = I2C_OK;
    
    // Initialize
    dacInit();
    uartDriverInit(UART1_BAUD, &onUartReadDone);
    i2cDriverInit(&i2cPackage , &onI2cDone);
    adcInit(&onAdcReadDone);
    
    dacEnable(true);
    uartDriverEnable(DEBUG);
    i2cDriverEnable(true);
    
    // Default
    dacSetValueA(*setVoltage);
    dacSetValueB(*setCurrent);
    
    DelayMs(10);
    if (DEBUG) printf("start\n");
    
    // Enable
    adcEnable(true);
    clipEnable(true);
    
    DelayMs(10);
    
    // TEST
    //dacSetVoltageA(1);
    // TEST
    LED1 = 1;
    
    while(1) {
        
        LED2 = !LED2;
        DelayMs(500);
        LED1 = !LED1;
        DelayMs(500);
       
        if (i2cDone) {
            i2cDone = false;
            // TODO
        }
        
        if (adcDone) {
            adcDone = false;
            // TODO
        }
    }
}

//  Clip interrupt
void __attribute__ ( (interrupt, no_auto_psv) ) _CNInterrupt(void) {
    if (_CNIF) {
        CLIP_LED = !CLIP_PIN;
        _CNIF = 0; // Clear interrupt
    }
}

void onI2cDone(i2cPackage_t data) {
    
}

void onAdcReadDone(uint16_t buffer, uint16_t * data) {
//    msrVoltage = data.value0;
//    msrCurrent = data.value1;
//    msrTemperature = data.value2;
//
//    adcEnable(true); // Restart AD conversion
//    adcDone = true;
}

