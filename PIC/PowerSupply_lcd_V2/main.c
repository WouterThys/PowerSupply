#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <string.h>
#include <math.h>

#include "../Common/COM_Settings.h"
#include "Settings.h"

#include "Drivers/SYSTEM_Driver.h"
#include "Drivers/I2C_Driver.h"
#include "Drivers/SPI_Driver.h"
#include "Drivers/LCD_Driver.h"
#include "Drivers/ENC_Driver.h"
#include "menu.h"
#include "utils.h"


/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
#define VOLTAGE_STEP 100
#define MENU_MAX_ON 1000

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/

// Menu
static uint16_t menuOnCount = 0;

// Encoder
static int16_t encLast = -1;
static int16_t turns = 0;
static Button_t encButton;
static bool updateMenu = false;

// I²C
static i2cData_t i2cData;

// Regulator variables
static uint16_t varVoltage = 0;
static uint16_t varCurrent = 0;
static uint16_t msrVoltage = 0;
static uint16_t msrCurrent = 0;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize();
static void encTimerEnable(bool enable); 
static void menuTimerEnable(bool enable);

static bool checkI2cState(i2cData_t data);
static void writeI2cData(uint8_t command, uint16_t data);

static void encLogic();
static void menuLogic();


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
        case I2C_UNEXPECTED_ADR: 
        case I2C_STILL_BUSY: 
            LED1 = 1;
            return false;
    }
}

void writeI2cData(uint8_t command, uint16_t data) {
    split(data, &i2cData.data1, &i2cData.data2);
    i2cData.command = command;
    i2cMasterWrite(&i2cData);
    checkI2cState(i2cData);
}

/**
 * Timer which updates encoder every 1ms
 */
void encTimerEnable(bool enable) {
    T2CONbits.TON = 0; // Disable
    T2CONbits.TCS = 0; // Internal clock (Fp)
    T2CONbits.T32 = 0; // 16-bit timer
    T2CONbits.TCKPS = 0b11; // 1:256
    
    // Registers
    TMR2 = 0x0000;
    PR2 = 70;
    
    // Interrupts
    _T2IP = 1;
    _T2IF = 0; // Clear
    _T2IE = 1; // Enable
    
    if (enable) {
        T2CONbits.TON = 1; // Enable
    } 
}

/**
 * Timer which updates LCD every 10ms
 */
void menuTimerEnable(bool enable) {
    T4CONbits.TON = 0; // Disable
    T4CONbits.TCS = 0; // Internal clock (Fp)
    T4CONbits.T32 = 0; // 16-bit timer
    T4CONbits.TCKPS = 0b11; // 1:256
    
    // Registers
    TMR4 = 0x0000;
    PR4 = 700;
    
    // Interrupts
    _T4IP = 2;
    _T4IF = 0; // Clear
    _T4IE = 1; // Enable
    
    if (enable) {
        T4CONbits.TON = 1; // Enable
    }
}

void encLogic() {
    encService();
    int16_t encValue = encGetValue();
    encButton = encGetButton();
        
    // Knob turned
    if (encValue != encLast) {
        encLast = encValue;
        turns += encValue;
        if (encValue != 0) {
            updateMenu = true;
        }
    }
    
    // Knob pressed
    if (encButton != Open) { // TODO: to menuLogic()
        switch(encButton) {
            default:
            case Open: break;
            case Closed: break;
            case Pressed: break;
            case Held: break;
            case Released: break;
            
            case Clicked: 
                updateMenu = true;
                menuClicked(); 
                break;
            case DoubleClicked: 
                updateMenu = true;
                break;
        }
    }
}

void menuLogic() {
    uint16_t v, i;
    if (updateMenu) {
        menuTurnOnCursor(true);
        
        // Execute all turns
        while (turns != 0) {
            if (turns > 0) {
                menuTurn(-1);
                turns--;
            } else {
                menuTurn(1);
                turns++;
            }
        }
        
        menuGetVoltage(&v);
        menuGetCurrent(&i);
        
        // Voltage
        if (v != varVoltage) {
            varVoltage = v;
            writeI2cData(COM_SET_V, varVoltage);
        }
        
        // Current
        if (i != varCurrent) {
            varCurrent = i;
            writeI2cData(COM_SET_I, varCurrent);
        }    
        
        menuOnCount = 0;
        updateMenu = false;
    } else {
        if (menuOnCount < MENU_MAX_ON) {
            menuOnCount++;
        }
    }
    
    // Request data
    i2cData.command = COM_A1;
    i2cMasterRead(&i2cData);
    concatinate(i2cData.data1, i2cData.data2, &v);
    if (msrVoltage>>4 != v>>4) {
        msrVoltage = v;
        menuSetMeasuredVoltage(msrVoltage);
    }
    
    
    i2cData.command = COM_A0;
    i2cMasterRead(&i2cData);
    concatinate(i2cData.data1, i2cData.data2, &i);
    if (msrCurrent>>4 != i>>4) {
        msrCurrent = i;
        menuSetMeasuredCurrent(msrCurrent);
    }
    
    // Menu on count
    if (menuOnCount >= MENU_MAX_ON) {
        menuTurnOnCursor(false);
    }
}

/*******************************************************************************
 *          MAIN PROGRAM
 ******************************************************************************/

int main(void) {

    initialize();
    
    DelayMs(1);
    menuInit();
    encInit();
    i2cInitMaster();
    
    DelayMs(1);
    encTimerEnable(true);
    menuTimerEnable(true);
    i2cEnable(true);
    
    varVoltage = 0;
    varCurrent = 0;
    i2cData.address = VARIABLE_ADDRESS;
    
    DelayMs(1);
    writeI2cData(COM_SET_V, varVoltage);
    writeI2cData(COM_SET_I, varCurrent);
    
    while (1) {
        
    }
    return 0;
}


void __attribute__ ( (interrupt, no_auto_psv) ) _T2Interrupt(void) {
    if (_T2IF) {
        encLogic();
        _T2IF = 0; // Clear interrupt
    }
}

void __attribute__ ( (interrupt, no_auto_psv) ) _T4Interrupt(void) {
    if (_T4IF) {
        menuLogic();
        _T4IF = 0; // Clear interrupt
    }
}
