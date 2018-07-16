#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <string.h>
#include <p33EP256GP502.h>

#include "utils.h"
#include "Settings.h"
#include "../Common/COM_Settings.h"

#include "Drivers/SYSTEM_Driver.h"
#include "Drivers/UART_Driver.h"
#include "Drivers/I2C_Driver.h"
#include "Drivers/ADC_Driver.h"

#include "Controllers/DAC_Controller.h"
#include "Controllers/PID_Controller.h"


/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
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
static bool adcDone = false;
static bool i2cDone = false;
static i2cPackage_t i2cPackage;

/**
 * [0] - setVoltage [mV]
 * [1] - setCurrent [mA]
 * [2] - msrVoltage [mV]
 * [3] - msrCurrent [mA]
 * [4] - msrTemperature [°C]
 */
static uint16_t dataArray[5];

// Pointers for convenience
static uint16_t * setVoltage;
static uint16_t * setCurrent;
static uint16_t * msrVoltage;
static uint16_t * msrCurrent;
static uint16_t * msrTemperature;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize();
static void clipEnable(bool enable);
static void ledTimerEnable(bool enable);

static void onI2cDone(i2cPackage_t data);
static void onAdcReadDone(uint16_t buffer, uint16_t * data);
static void onUartReadDone(UartData_t data);

static void voltageSweep();

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

void onUartReadDone(UartData_t data) {
    
}



/*******************************************************************************
 *          MAIN PROGRAM
 ******************************************************************************/

int main(void) {
    initialize();
    
    setVoltage = &dataArray[0];
    setCurrent = &dataArray[1];
    msrVoltage = &dataArray[2];
    msrCurrent = &dataArray[3];
    msrTemperature = &dataArray[4];
    
    i2cPackage.data = dataArray;
    i2cPackage.length = 5;
    
    // Initialize
    uartDriverInit(UART1_BAUD, &onUartReadDone);
    i2cDriverInit(&i2cPackage, &onI2cDone);
    dacInit();
    adcInit(&onAdcReadDone);
    
    // Enable
    uartDriverEnable(DEBUG);
    i2cDriverEnable(true);
    adcEnable(true);
    dacEnable(true);
    clipEnable(true);
    
    // Default
    dacSetValueA(*setVoltage);
    dacSetValueB(*setCurrent);
    
    DelayMs(10);
    if (DEBUG) printf("start\n");
    DelayMs(10);
    
    while(1) {
        
        if (i2cDone) {
            i2cDone = false;
            
            dacSetValueA(*setVoltage);
            dacSetValueB(*setCurrent);
        }
        
        if (adcDone) {
            adcDone = false;
            
            // PID
//            if (pidMode == MANUAL) {
//                pidSetMode(MANUAL);
//                
//                pidCompute(varVoltage, msrVoltage, &pidOutput);
//                dacSetVoltageA(( ((float)varVoltage) / 5000));
//                pidMode = AUTOMATIC;
//            } else {
//                pidSetMode(AUTOMATIC);
//                
//                pidCompute(varVoltage, msrVoltage, &pidOutput);
//                dacSetValueA(pidOutput);
//            }
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
    if (data.status == I2C_MWRITE) {
        i2cDone = true;
    }
}

void onAdcReadDone(uint16_t buffer, uint16_t * data) {
    // TODO: average of the buffers -> set msr data
//    msrVoltage = data.value0;
//    msrCurrent = data.value1;
//    msrTemperature = data.value2;
//
//    adcEnable(true); // Restart AD conversion
//    adcDone = true;
}

void voltageSweep() {
//    uint16_t cnt = 0;
//    uint16_t voltage = 0;
//    
//    while(cnt < TEST_BUFFER_CNT) { 
//
//        dacSetVoltageA(( ((float)voltage) / 5000));
//        DelayMs(500);
//        
//        adcEnable(true); // Restart AD conversion
//        
//        while(!adcDone);
//        adcDone = false;
//        
//        testBuffer[cnt].setVoltage = voltage;
//        testBuffer[cnt].setCurrent = 0;
//        testBuffer[cnt].msrVoltage = msrVoltage;
//        testBuffer[cnt].msrCurrent = msrCurrent;
//        testBuffer[cnt].clip = 0;
//        
//        cnt++;
//        voltage = cnt * TEST_VOLTAGE_STEP;
//    }
//    
//    cnt = 0;
//    while(cnt < TEST_BUFFER_CNT) { 
//        
//        printf("%d,%d,%d,%d,%d\n", 
//                testBuffer[cnt].setVoltage,
//                testBuffer[cnt].setCurrent,
//                testBuffer[cnt].msrVoltage,
//                testBuffer[cnt].msrCurrent,
//                testBuffer[cnt].clip);
//        
//        DelayUs(100);
//        
//        cnt++;
//    }
}
