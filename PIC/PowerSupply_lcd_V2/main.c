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
#include "Drivers/UART_Driver.h"
#include "menu.h"
#include "utils.h"


/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
#define VOLTAGE_STEP 100
#define MENU_MAX_ON 1000
#define UART_BUFFER   10

// Main commands 0x0X
#define C_START             0x01
#define C_HANDSHAKE         0x02

#define C_SET_VOLTAGE       0x10
#define C_SET_CURRENT       0x11
#define C_GET_VOLTAGE       0x12
#define C_GET_CURRENT       0x13

// LCD commands 0x1X
#define C_LCD_BRIGHTNESS    0x70
#define C_LCD_CONTRAST      0x71


#define M_START     "SS"

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

// Communication
static i2cData_t i2cData;
static UartData_t uartDataBuffer[UART_BUFFER];
static int16_t uartDataCnt = -1;

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
static void encLogic();

static void menuTimerEnable(bool enable);
static void menuLogic();

static bool i2cCheckState(i2cData_t data);
static void i2cWriteData(uint8_t command, uint16_t data);

static bool uartCheckState(UartData_t data);
static bool uartPutData(UartData_t data);
static bool uartGetData(UartData_t * data);

static void setVoltage(uint16_t voltage);
static void setCurrent(uint16_t current);


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

void onUartReadDone(UartData_t data) {
    if (uartCheckState(data)) {
        LED1 = 0;
        if (!uartPutData(data)) {
            // TODO: do something with error
        }
    } else {
        LED1 = 1;
    }
}

/*******************************************************************************
 *          MAIN PROGRAM
 ******************************************************************************/
int main(void) {

    initialize();
    
    uartInit(UART1_BAUD, &onUartReadDone);
    menuInit();
    encInit();
    i2cInitMaster();
    
    DelayMs(100);
    uartEnable(true);
    encTimerEnable(true);
    menuTimerEnable(true);
    i2cEnable(true);
    
    varVoltage = 0;
    varCurrent = 0;
    i2cData.address = VARIABLE_ADDRESS;
    
    DelayMs(100);
    
    i2cWriteData(COM_SET_V, varVoltage);
    i2cWriteData(COM_SET_I, varCurrent);
    
    DelayMs(1000);
    uartWrite(C_START, M_START);
    
    
    UartData_t uartData;
    while (1) {
        
        if (uartGetData(&uartData)) {
            LED2 = !LED2;
            uint16_t value;
            switch (uartData.command) {
                // Basic
                case C_HANDSHAKE:
                    break;
                    
                // Supply stuff
                case C_SET_VOLTAGE:
                    concatinate(uartData.message[0], uartData.message[1], &value);
                    setVoltage(value);
                    menuSetVariableVoltage(value);
                    break;
                case C_SET_CURRENT:
                    concatinate(uartData.message[0], uartData.message[1], &value);
                    setCurrent(value);
                    menuSetVariableCurrent(value);
                    break;
                case C_GET_VOLTAGE:
                    
                    break;
                case C_GET_CURRENT:
                    
                    break;
                    
                // LCD stuff
                case C_LCD_CONTRAST:
                    lcdSetDisplayContrast(uartData.message[0]); // 1 - 50
                    break;
                case C_LCD_BRIGHTNESS:
                    lcdSetDisplayBrightness(uartData.message[0]); // 1 - 8
                    break;

                // Voltage & current stuff
            }
        }
        
    }
    return 0;
}

/*******************************************************************************
 *          Supply control
 ******************************************************************************/
void setVoltage(uint16_t voltage) {
    varVoltage = voltage;
    i2cWriteData(COM_SET_V, varVoltage);
}

void setCurrent(uint16_t current) {
    varCurrent = current;
    i2cWriteData(COM_SET_I, varCurrent);
}


/*******************************************************************************
 *          I²C
 ******************************************************************************/
bool i2cCheckState(i2cData_t data) {
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
            return false;
    }
}

void i2cWriteData(uint8_t command, uint16_t data) {
    split(data, &i2cData.data1, &i2cData.data2);
    i2cData.command = command;
    i2cMasterWrite(&i2cData);
    i2cCheckState(i2cData);
}

/*******************************************************************************
 *          ENCODER
 ******************************************************************************/
void __attribute__ ( (interrupt, no_auto_psv) ) _T2Interrupt(void) {
    if (_T2IF) {
        encLogic();
        _T2IF = 0; // Clear interrupt
    }
}

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


/*******************************************************************************
 *          MENU
 ******************************************************************************/
void __attribute__ ( (interrupt, no_auto_psv) ) _T4Interrupt(void) {
    if (_T4IF) {
        menuLogic();
        _T4IF = 0; // Clear interrupt
    }
}

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
            setVoltage(v);
        }
        
        // Current
        if (i != varCurrent) {
            setCurrent(i);
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
 *          UART
 ******************************************************************************/
bool uartCheckState(UartData_t data) {
    switch(data.status) {
        default:
        case STA_OK             : return true;
        case STA_NOK            : return false;
        case STA_INVALID_START  : return false;
        case STA_INVALID_STOP   : return false;
        case STA_INVALID_SEP    : return false;
        case STA_INVALID_LENGTH : return false;
        case STA_INVALID_COMMAND: return false;
        case STA_INVALID_MESSAGE: return false;
    }
}

bool uartPutData(UartData_t data) {
    if (uartDataCnt >= UART_BUFFER) {
        return false;
    }
    
    uartDataCnt++;
    uartDataBuffer[uartDataCnt] = data;
    return true;
}

bool uartGetData(UartData_t * data) {
    if (uartDataCnt < 0) {
        return false;
    }
    
    *data = uartDataBuffer[0]; // Always take first element
    
    // Shift
    uint16_t i;
    for (i = 1; i <= uartDataCnt; i++) {
        uartDataBuffer[i-1] = uartDataBuffer[i];
    }
    uartDataCnt--;
    
    return true;
}
