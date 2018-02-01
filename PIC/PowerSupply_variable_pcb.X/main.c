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
#include "Controllers/MCP4131_Controller.h"
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
    
    C_DAC_Init(); // DAC and ADC
    C_DAC_Enable(true);
    
    D_ADC_Init(&adcBuffer);
    
    
    D_UART_Init(UART_MODULE_1, UART1_BAUD);
    D_UART_Enable(UART_MODULE_1, true);
    
    DelayMs(1000);
    C_DAC_Write(400); // 0.value * 2 * 4
    
    while(1) {
    
        if (UART_flag) {
            LED1 = 1;
            UART_flag = false;
            READ_Data read = C_UART_Read();
            if (strcmp(read.command, "S") == 0) {
                D_ADC_Enable(true);
            } else if (strcmp(read.command, "V") == 0) {
                uint16_t val = atoi(read.message);
                C_DAC_Write(val);
            }
        }
        
        if (ADC_flag) {
            C_UART_WriteInt("A0", adcBuffer.value0);
            DelayMs(10);
            C_UART_WriteInt("A1", adcBuffer.value1);
            DelayMs(10);
            C_UART_WriteInt("A2", adcBuffer.value2);
            DelayMs(10);
            C_UART_WriteInt("A3", adcBuffer.value3);

            
            ADC_flag = false;
            DelayMs(1000);
            D_ADC_Enable(true);
        }
        
    }
}


