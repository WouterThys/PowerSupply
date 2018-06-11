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
typedef struct {
    uint16_t setVoltage;
    uint16_t msrVoltage;
    uint16_t setCurrent;
    uint16_t msrCurrent;
    bool clip;
} Measure_t;


/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static uint16_t setVoltage;     // Voltage send to DAC in mV
static uint16_t varVoltage;     // Desired voltage in mV

static uint16_t setCurrent;     // Voltage send to DAC in mV
static uint16_t varCurrent;     // Desired max current

static uint16_t msrVoltage;     // Measured voltage
static uint16_t msrCurrent;     // Measured current
static uint16_t msrTemperature; // Measured temperature

static bool adcDone = false;
static bool i2cDone = false;
static uint16_t i2cError = 0;

static uint16_t pidMode = AUTOMATIC;
static float pidOutput;

Measure_t testBuffer[TEST_BUFFER_CNT];

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize();
static void clipEnable(bool enable);

static void ledTimerEnable(bool enable);
static bool checkI2cState(i2cData_t data);

static void onI2cMasterAnswer(i2cData_t * data);
static void onI2cReadDone(i2cData_t data);
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

bool checkI2cState(i2cData_t data) {
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
    
    // Initialize
    dacInit();
    uartDriverInit(UART1_BAUD, &onUartReadDone);
    i2cInitSlave(ADDR_VAR, &onI2cMasterAnswer, &onI2cReadDone);
    adcInit(&onAdcReadDone);
    
    dacEnable(true);
    uartDriverEnable(DEBUG);
    i2cEnable(true);
//    pidSetTunings(60,   0.25, 0.002, 0.008,   0, 4095); 
    
    // Default
    varVoltage = 0;
    varCurrent = 4095;
    setVoltage = 0;
    setCurrent = 4095;
    dacSetValueA(varVoltage);
    dacSetValueB(varCurrent);
    
    DelayMs(10);
    if (DEBUG) printf("start\n");
    
    // Enable
    adcEnable(true);
    //ledTimerEnable(true);
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
            if (setVoltage != varVoltage) {
                dacSetVoltageA(toDACvoltage(setVoltage));
                varVoltage = setVoltage;
            }
            
            if (setCurrent != varCurrent) {
                dacSetVoltageB(toDACcurrent(setCurrent));
                varCurrent = setCurrent;
            }
            i2cDone = false;
            
            if (DEBUG) {
                printf("\n");
                printf(" V = %d\n", setVoltage);
                printf(" I = %d\n", setCurrent);
                printf(" v = %d\n", (uint16_t)fromADCtoVoltage(msrVoltage));
                printf(" i = %d\n", (uint16_t)fromADCtoCurrent(msrCurrent));
            }
            
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

void onI2cMasterAnswer(i2cData_t * data) {
    uint16_t value = 0;
    switch(data->command) {
        case COM_GET_V:
            value = (uint16_t)fromADCtoVoltage(msrVoltage);
            split(value, &data->data1, &data->data2);
            break;
        case COM_GET_I:
            value = (uint16_t)fromADCtoCurrent(msrCurrent);
            split(value, &data->data1, &data->data2);
            break;
        case COM_GET_T:
            value = (uint16_t)fromADCtoTemperature(msrTemperature);
            split(value, &data->data1, &data->data2);
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
            i2cDone = true;
        }

        // Set current
        if (data.command == COM_SET_I) {
            concatinate(data.data1, data.data2, &setCurrent);
            i2cDone = true;
        }
        
    }
}

void onAdcReadDone(uint16_t buffer, uint16_t * data) {
//    msrVoltage = data.value0;
//    msrCurrent = data.value1;
//    msrTemperature = data.value2;
//
//    adcEnable(true); // Restart AD conversion
//    adcDone = true;
    
    if (DEBUG && buffer == 1) {
        LED1 = 1;
        
        int i = 0;
        for (i=0; i<8; i++) {
            printf("%d\n", *(data + 1));
        }
        
        LED1 = 0;
        
    }
}

void voltageSweep() {
    uint16_t cnt = 0;
    uint16_t voltage = 0;
    
    while(cnt < TEST_BUFFER_CNT) { 

        dacSetVoltageA(( ((float)voltage) / 5000));
        DelayMs(500);
        
        adcEnable(true); // Restart AD conversion
        
        while(!adcDone);
        adcDone = false;
        
        testBuffer[cnt].setVoltage = voltage;
        testBuffer[cnt].setCurrent = 0;
        testBuffer[cnt].msrVoltage = msrVoltage;
        testBuffer[cnt].msrCurrent = msrCurrent;
        testBuffer[cnt].clip = 0;
        
        cnt++;
        voltage = cnt * TEST_VOLTAGE_STEP;
    }
    
    cnt = 0;
    while(cnt < TEST_BUFFER_CNT) { 
        
        printf("%d,%d,%d,%d,%d\n", 
                testBuffer[cnt].setVoltage,
                testBuffer[cnt].setCurrent,
                testBuffer[cnt].msrVoltage,
                testBuffer[cnt].msrCurrent,
                testBuffer[cnt].clip);
        
        DelayUs(100);
        
        cnt++;
    }
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




