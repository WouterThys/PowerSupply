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
    D_INT_EnableInterrupts(false);
    
    // Initialize system
    D_SYS_InitPll();
    D_SYS_InitOscillator();
    D_SYS_InitPorts();
    
    // Interrupts
    D_INT_Init();
    D_INT_EnableInterrupts(true);
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
    C_DAC_Init();
    D_UART_Init(UART_MODULE_1, UART1_BAUD);
    D_ADC_Init(&adcBuffer);
    
    // Enable
    C_DAC_Enable(true);
    D_UART_Enable(UART_MODULE_1, true);
    D_ADC_Enable(true);

    DelayMs(1000);
    
    C_DAC_SetVoltageA(1);
    C_DAC_SetVoltageB(2);
    
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
            printf("A0: %d\n", adcBuffer.value0);
            DelayMs(10);
            printf("A1: %d\n", adcBuffer.value1);
            DelayMs(10);
            printf("A2: %d\n", adcBuffer.value2);
            DelayMs(10);
            printf("A3, %d\n\n", adcBuffer.value3);

            
            ADC_flag = false;
            D_ADC_Enable(true);
        }
        
    }
}


