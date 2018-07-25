#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <math.h>

#include "../Settings.h"
#include "../Drivers/SPI2_Driver.h"
#include "DAC_Controller.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
#define N       pow(2, DAC_n)

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
DAC_Command_t DAC_A;
DAC_Command_t DAC_B;

/*******************************************************************************
 *          BASIC FUNCTIONS
 ******************************************************************************/
static uint16_t voltageToDigital(float voltage, float gain);
static void writeToDac(DAC_Command_t command);


uint16_t voltageToDigital(float voltage, float gain) {
    return (uint16_t) ((voltage * N) / (DAC_VREF * gain));
}

void writeToDac(DAC_Command_t command) {
    uint16_t d = 0x0000;
    d |= command.A_B  << 15;
    d |= command.BUF  << 14;
    d |= command.GA   << 13;
    d |= command.SHDN << 12;
    d |= command.data;
    
    spi2Write(d);
}

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/

void dacInit() {
    spi2Init();
    
    // Settings
    DAC_A.A_B = 0;      // Channel A
    DAC_A.BUF = 1;      // Buffered reference
    DAC_A.GA = 1;       // 1x (Vout = Vref * D / 4096)
    DAC_A.SHDN = 0;     // Disable
    DAC_A.data = 0x000; // Data
    
    DAC_B.A_B = 1;      // Channel B
    DAC_B.BUF = 1;      // Buffered reference
    DAC_B.GA = 1;       // 1x
    DAC_B.SHDN = 0;     // Disable
    DAC_B.data = 0x000;
}

void dacEnable(bool enable) {
    spi2Enable(enable);
    
    if (enable) {
        DAC_A.SHDN = 1;
        DAC_B.SHDN = 1;
    } else {
        DAC_A.SHDN = 0;
        DAC_B.SHDN = 0;
    }
}

void dacInitDacA(bool buf, uint16_t gain) {
    DAC_A.BUF = buf;
    if (gain == 1) DAC_A.GA = 1;
    if (gain == 2) DAC_A.GA = 0;
}

void dacInitDacB(bool buf, uint16_t gain) {
    DAC_B.BUF = buf;
    if (gain == 1) DAC_B.GA = 1;
    if (gain == 2) DAC_B.GA = 0;
}

void dacShutDownA() {
    DAC_A.SHDN = 0;
}

void dacShutDownB() {
    DAC_B.SHDN = 0;
}

void dacSetValueA(uint16_t value) {
    if (value > (N-1)) {
        value = (N-1);
    }
    if (value < 0) {
        value = 0;
    }
    DAC_A.data = value;
    writeToDac(DAC_A);
}

void dacSetValueB(uint16_t value) {
    if (value > (N-1)) {
        value = (N-1);
    }
    if (value < 0) {
        value = 0;
    }
    DAC_B.data = value;
    writeToDac(DAC_B);
}

void dacSetVoltageA(float voltage) {
    if (DAC_A.GA == 0) {
        DAC_A.data = voltageToDigital(voltage, 2);
    } else {
        DAC_A.data = voltageToDigital(voltage, 1);
    }
    
    writeToDac(DAC_A);
}

void dacSetVoltageB(float voltage) {
    if (DAC_B.GA == 0) {
        DAC_B.data = voltageToDigital(voltage, 2);
    } else {
        DAC_B.data = voltageToDigital(voltage, 1);
    }
    
    writeToDac(DAC_B);
}



