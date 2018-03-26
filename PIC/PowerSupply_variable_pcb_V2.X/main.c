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
#define PID_TEST_CNT    128
/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
uint16_t setVoltage; // Voltage send to DAC in mV
uint16_t varVoltage; // Desired voltage in mV
uint16_t varCurrent; // Maximum current in mA

float msrVoltage; // Measured voltage
float msrCurrent; // Measured current

bool adcDone = false;
uint16_t pidMode = AUTOMATIC;
float pidOutput;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize();

static void ledTimerEnable(bool enable);
static bool checkI2cState(i2cData_t data);

static void onI2cMasterAnswer(i2cData_t * data);
static void onI2cReadDone(i2cData_t data);
static void onAdcReadDone(AdcBuffer_t data);

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

bool checkI2cState(i2cData_t data) {
    switch(data.status) {
        default: 
            return true;
        case I2C_NOK: 
        case I2C_OVERFLOW: 
        case I2C_COLLISION: 
        case I2C_NO_ADR_ACK: 
        case I2C_NO_DATA_ACK: 
        case I2C_UNEXPECTED_DATA: 
            i2cReset();
            return false;
        case I2C_UNEXPECTED_ADR: 
        case I2C_STILL_BUSY: 
            LED1 = 1;
            return false;
    }
}


/*******************************************************************************
 *          MAIN PROGRAM
 ******************************************************************************/

int main(void) {
    initialize();
    TRISBbits.TRISB5 = 1;
    
    // Initialize
    dacInit();
    uartInit(UART_MODULE_1, UART1_BAUD);
    i2cInitSlave(VARIABLE_ADDRESS, &onI2cMasterAnswer, &onI2cReadDone);
    adcInit(&onAdcReadDone);
    
    dacEnable(true);
    uartEnable(UART_MODULE_1, true);
    i2cEnable(true);
    pidSetTunings(60,   0.25, 0.002, 0.008,   0, 4095); 
    
    // Default
    varVoltage = 0;
    varCurrent = 0;
    setVoltage = 0;
    dacSetValueA(varVoltage);
    dacSetValueB(varCurrent);
    
    printf("start\n");
    
    // Enable
    adcEnable(true);
    ledTimerEnable(true);
    
    while(1) {
        if (adcDone) {
            adcDone = false;
            
            if (setVoltage != varVoltage) {
                dacSetVoltageA(( ((float)setVoltage) / 5000));
                varVoltage = setVoltage;
            }
            
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

// Timer 2 interrupt
void __attribute__ ( (interrupt, no_auto_psv) ) _T2Interrupt(void) {
    if (_T2IF) {
        
        _T2IF = 0; // Clear interrupt
    }
}

void onI2cMasterAnswer(i2cData_t * data) {
    switch(data->command) {
        case COM_GET_I:
            split((uint16_t)msrCurrent, &data->data1, &data->data2);
            break;
        case COM_GET_V:
            split((uint16_t)msrVoltage, &data->data1, &data->data2);
            break;
        default:
            break;
    }
}

void onI2cReadDone(i2cData_t data) {
    // Check state
    if (checkI2cState(data)) {
        // Set voltage
        if (data.command == COM_SET_V) {
            concatinate(data.data1, data.data2, &setVoltage);
            //pidMode = MANUAL;
        }

        // Set current
        if (data.command == COM_SET_I) {
            concatinate(data.data1, data.data2, &varCurrent);
            //dacSetVoltageB(( ((double)varCurrent) / 5000));
        }
    }
}

void onAdcReadDone(AdcBuffer_t data) {
    msrCurrent = ((float)adcValueToVolage(data.value0)) * 500; // in mA
    msrVoltage = ((float)adcValueToVolage(data.value1)) * 4830;// in mV

    adcEnable(true); // Restart AD conversion
    adcDone = true;
}






//void printCalibration(uint16_t seq, uint16_t varVoltage, double msrVoltage) {
//
//        // 2048 / 16 = 128 values
//        if (!calibrateAllDone && calibrateAdcDone) {
//            calibrateAdcDone = false;
//            if(varVoltage < 10) {
//                dacSetVoltageA( (double)varVoltage / 5);
//                
//                DelayMs(1000);
//                adcEnable(true);
//            } else {
//                calibrateAllDone = true;
//            }
//        }
//        
//        
//        // ADC
//        if (ADC_flag) {
//            if (!calibrateAllDone) {
//                uint16_t v = adcBuffer.value1;
//
//                printf("%d,%d,%d\n", calibrateSequence, varVoltage, v);
//
//                calibrateAdcDone = true;
//                calibrateSequence++;
//                varVoltage++;
//            }
//            ADC_flag = false;
//        } 
//}


//        if (UART_flag) {
//            LED1 = 1;
//            UART_flag = false;
//            READ_Data read = C_UART_Read();
//            if (strcmp(read.command, "S") == 0) {
//                D_ADC_Enable(true);
//            } else if (strcmp(read.command, "V") == 0) {
//                uint16_t val = atoi(read.message);
//                C_DAC_Write(val);
//            }
//        }



//            printf("ISense: %e\n", adcValueToVolage(adcBuffer.value0));
//            DelayMs(10);
//            printf("%e\n", adcValueToVolage(adcBuffer.value1) / (varVoltage / 1000));
//            DelayMs(10);
//            printf("Temp: %e\n", adcValueToVolage(adcBuffer.value2));
//            DelayMs(10);
//            printf("IMon, %e\n\n", adcValueToVolage(adcBuffer.value3));




