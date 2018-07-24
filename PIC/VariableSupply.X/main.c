#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <string.h>

#include "utils.h"
#include "Settings.h"
#include "../Common/CommonData.h"

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


/*******************************************************************************
 *          DEFINES
 ******************************************************************************/


/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static uint16_t dataArray[7];
static uint16_t * setVoltage;
static uint16_t * setCurrent;
static uint16_t * msrVoltage;
static uint16_t * msrCurrent;
static uint16_t * msrTemperature;
static uint16_t * msrCurrent_;
static SupplyStatus_t * status;

static i2cPackage_t i2cPackage;
static int16_t i2cError;

static uint16_t i2cDone = 0;
static bool adcDone = false;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize();
static void clipEnable(bool enable);

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
    msrCurrent_ = &dataArray[I2C_COM_MSR_I_];
    status = (SupplyStatus_t *) &dataArray[I2C_COM_STATUS];
    
    *setVoltage = 0x0000;
    *setCurrent = 0x0FFF; // Max
    *msrVoltage = 0x0000;
    *msrCurrent = 0x0000;
    *msrTemperature = 0x0000;
    *msrCurrent_ = 0x0000;
    
    status->statusCode = STAT_VOID;
    status->currentClip = 0;
    status->errorCode = 0;
    status->outputEnabled = 0; 
    
    // Initial values
    i2cPackage.address = I2C_ADDRESS;
    i2cPackage.data = dataArray;
    i2cPackage.length = 5;
    i2cError = I2C_OK;
    
    // Initialize
    dacInit();
    uartDriverInit(UART1_BAUD, &onUartReadDone);
    i2cDriverInit(&i2cPackage , &onI2cDone);
    adcDriverInit(&onAdcReadDone);
    OUTPUT_ON_Dir = 1; // Input to check if output is enabled
    
    dacEnable(true);
    uartDriverEnable(DEBUG);
    i2cDriverEnable(true);
    DelayMs(100);
    
    // Default
    dacSetValueA(*setVoltage);
    dacSetValueB(*setCurrent);
    
    DelayMs(10);
    if (DEBUG) printf("start\n");
    
    // Enable
    adcDriverEnable(true);
    clipEnable(true);
    
    // Set status running
    status->statusCode = STAT_RUNNING;
    
    while(1) {
       
        if (i2cDone > 0) {
            
            if (DEBUG_I2C) printf("I2C: %d\n", i2cDone);
            
            switch(i2cDone) {
                case I2C_COM_SET_V:
                    dacSetValueA(*setVoltage);
                    break;
                case I2C_COM_SET_I:
                    dacSetValueB(*setCurrent);
                    break;
                case I2C_COM_STATUS:
                    break;
                default:
                    break;
            }
            
            i2cDone = 0;
        }
        
        if (adcDone) {
            adcDone = false;
            // TODO
            
            status->outputEnabled = OUTPUT_ON_Pin;
            LED1 = !LED1;
        }
    }
}

//  Clip interrupt
void __attribute__ ( (interrupt, no_auto_psv) ) _CNInterrupt(void) {
    if (_CNIF) {
        bool clip = !CLIP_PIN;
        status->currentClip = clip;
        CLIP_LED = clip;
        _CNIF = 0; // Clear interrupt
    }
}

void onI2cDone(i2cPackage_t data) {
    // We are only interested in what master writes
    if (data.status == I2C_MWRITE) { 
        i2cDone = data.command - 1; // Command will be pointing to next element by now => - 1
    }
}

void onAdcReadDone(uint16_t buffer, uint16_t * data) {
    dataArray[I2C_COM_MSR_V + buffer] = (uint16_t) average(data, ADC_BUFFER_SIZE);
    adcDone = true;
}

