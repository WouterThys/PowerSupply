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
    D_UART_Init(UART_MODULE_1, UART1_BAUD);
    adcInit(&adcBuffer);
    
    // Enable
    dacEnable(true);
    D_UART_Enable(UART_MODULE_1, true);
    adcEnable(true);

    DelayMs(1000);
    
    dacSetVoltageA(1.5);
    dacSetVoltageB(0);
    
    printf("start\n");
    
    while(1) {
        
        LED1 = !LED1;
        DelayMs(200);
        
        LED2 = !LED2;
        DelayMs(200);
        
        LED3 = !LED3;
        DelayMs(200);
    
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
            printf("ISense: %e\n", adcValueToVolage(adcBuffer.value0));
            DelayMs(10);
            printf("VSense: %e\n", adcValueToVolage(adcBuffer.value1));
            DelayMs(10);
            printf("Temp: %e\n", adcValueToVolage(adcBuffer.value2));
            DelayMs(10);
            printf("IMon, %e\n\n", adcValueToVolage(adcBuffer.value3));

            
            ADC_flag = false;
            adcEnable(true);
        }
        
    }
}


