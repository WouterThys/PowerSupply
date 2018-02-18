#if defined(__XC16__)
#include <xc.h>
#elif defined(__C30__)
#if defined(__dsPIC33E__)
#include <p33Exxxx.h>
#elif defined(__dsPIC33F__)
#include <p33Fxxxx.h>
#endif
#endif
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <string.h>
#include <math.h>

#include "Settings.h"

#include "Drivers/INTERRUPT_Driver.h"
#include "Drivers/SYSTEM_Driver.h"
#include "Drivers/UART_Driver.h"
#include "Drivers/SPI_Driver.h"
#include "Drivers/LCD_Driver.h"
#include "Drivers/ENC_Driver.h"
#include "menu.h"
#include "utils.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
#define VOLTAGE_STEP 100


/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/

// Encoder
int16_t encLast = -1;
int16_t encValue = 0;
Button_t encButton;
bool updateMenu = false;

// Variables
int16_t varVoltage = 0;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize();
static void setEncTimer(); 
static void setMenuTimer();

static void encLogic();
static void menuLogic();


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

/**
 * Timer which updates encoder every 1ms
 */
void setEncTimer() {
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
    
    T2CONbits.TON = 1; // Enable
}

/**
 * Timer which updates LCD every 10ms
 */
void setMenuTimer() {
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
    
    T4CONbits.TON = 1; // Enable
}

void encLogic() {
    encService();
    encValue = encGetValue();
    encButton = encGetButton();
        
    if (encValue != encLast) {
        encLast = encValue;
        if (encValue != 0) {
            //printf("E:%d\n", encValue);
            varVoltage += (VOLTAGE_STEP * encValue);
            updateMenu = true;
        }
    }
//
//    if (encButton != Open) {
//        switch(encButton) {
//            default:
//            case Closed: printf("B:C\n"); break;
//            case Pressed: printf("B:P\n"); break;
//            case Held: printf("B:H\n"); break;
//            case Released: printf("B:R\n"); break;
//            case Clicked: printf("B:L\n"); break;
//            case DoubleClicked: printf("B:D\n"); break;
//        }
//    }
}

void menuLogic() {
    
}

/*******************************************************************************
 *          MAIN PROGRAM
 ******************************************************************************/

int main(void) {

    initialize();
    
    D_UART_Init(UART_MODULE_1, 57600);
    D_UART_Enable(UART_MODULE_1, true);
    
    DelayMs(1);
    menuInit();
    encInit();
    
    DelayMs(1);
    setEncTimer();
    //setMenuTimer();
   
    
    while (1) {
        if (updateMenu) {
            updateMenu = false;
            menuSetFieldValue(ID_VOLTAGE, varVoltage);
        }
        
    }
    return 0;
}


void __attribute__ ( (interrupt, no_auto_psv) ) _T2Interrupt(void) {
    if (_T2IF) {
        LED1 = !LED1;
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
