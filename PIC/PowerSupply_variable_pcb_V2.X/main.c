#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <string.h>

#include "Settings.h"
#include "../Common/COM_Settings.h"

#include "Drivers/INTERRUPT_Driver.h"
#include "Drivers/SYSTEM_Driver.h"

#include "Controllers/DAC_Controller.h"
#include "Drivers/ADC_Driver.h"

#include "Drivers/UART_Driver.h"
#include "Drivers/I2C_Driver.h"




/*******************************************************************************
 *          DEFINES
 ******************************************************************************/

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static AdcBuffer_t adcBuffer;

i2cData_t i2cData;
i2cAnswer_t i2cAnswer;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize();

void initialize() {
    intEnable(false);
    
    // Initialize system
    sysInitPll();
    sysInitOscillator();
    sysInitPorts();
    
    // Interrupts
    intInit();
    intEnable(true);
}

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/

/*******************************************************************************
 *          MAIN PROGRAM
 ******************************************************************************/

int main(void) {

    initialize();
    TRISBbits.TRISB5 = 1;
    
    // Initialize
    dacInit();
    uartInit(UART_MODULE_1, UART1_BAUD);
    i2cInitSlave(VARIABLE_ADDRESS, &i2cData, &i2cAnswer);
    adcInit(&adcBuffer);
    
    // Enable
    dacEnable(true);
    uartEnable(UART_MODULE_1, true);
    i2cEnable(true);
    //adcEnable(true);
    
    //dacSetVoltageA(1.5);
    //dacSetVoltageB(0);
    
    printf("start\n");
    
    while(1) {
        
        if (i2cReadyToRead) {
            i2cReadyToRead = false;
            
            printf("C: %d\n", i2cData.command);
            printf("S: %d\n", i2cData.status);
            printf("D0: %d\n", i2cData.data1);
            printf("D1: %d\n\n", i2cData.data2);
        }
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
//        
        if (ADC_flag) {
//            printf("ISense: %e\n", adcValueToVolage(adcBuffer.value0));
//            DelayMs(10);
//            printf("VSense: %e\n", adcValueToVolage(adcBuffer.value1));
//            DelayMs(10);
//            printf("Temp: %e\n", adcValueToVolage(adcBuffer.value2));
//            DelayMs(10);
//            printf("IMon, %e\n\n", adcValueToVolage(adcBuffer.value3));

            
            ADC_flag = false;
//            adcEnable(true);
        }
        
    }
}


