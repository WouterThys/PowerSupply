#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>

#include "../Settings.h"
#include "ENC_Driver.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/


/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static int16_t encTurnStates[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0}; 
static uint16_t encPressPosition = 0;
static int16_t encTurnPosition = 0;
static int16_t encTurnValue = 0;
static int16_t oldTurnEncValue = 0;
static uint16_t encPressedCount = 0;
static bool encPressed;

bool ENC_Change;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void encode();

void encode() {
    encTurnPosition <<= 2;  // Remember previous state
    encTurnPosition |= ((ENC_Pin_2 << 1) + (ENC_Pin_1)) & 0x0003; // LSB are new value
    encTurnValue += encTurnStates[(encTurnPosition & 0x000F)]; // Lower 4 bits as index of the states array
    
    encPressPosition <<= 1;
    encPressPosition |= ENC_Pin_3;
    
    if ((encPressPosition & 0x000F) == 0x000F) {
        if (!encPressed) {
            ENC_Change = true;
        }
        encPressed = true;
        encPressedCount++;
    } else if ((encPressPosition & 0x000F) == 0x0000) {
        if (encPressed) {
            ENC_Change = true;
            encPressedCount = 0;
        }
        encPressed = false;
    }
    
    if (encTurnValue == oldTurnEncValue) {
        return;
    } else {
        if ((encTurnValue % 2) == 0) {
            ENC_Change = true;
        }
    }
    
    oldTurnEncValue = encTurnValue;
}

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/

void D_ENC_Init() {
    D_ENC_Enable(false);
    
    /* Variables */
    ENC_Change = false;
    
    /* T2CON register */
    T2CONbits.TCKPS = 0b00;
    
    /* Interrupts */
    _T2IF = 0; // Clear
    _T2IP = T2_IP; // Priority
    _T2IE = 1; // Enable
}

void D_ENC_Enable(bool enable) {
    if (enable) {
        ENC_Dir_1 = 1;      // Input
        ENC_Dir_2 = 1;      // Input
        ENC_Dir_3 = 1;      // Input
        T2CONbits.TON = 1;  // Enable timer
    } else {
        ENC_Dir_1 = 0;      // Output
        ENC_Dir_2 = 0;      // Output
        ENC_Dir_3 = 0;      // Output
        T2CONbits.TON = 0;  // Disable timer
    }
}

void D_ENC_GetState(enc_t *state) {
    // Rotary
    if (encTurnValue >= 0) {
        state->turn = UP;
    } else {
        state->turn = DOWN;
    }
    
    if (encTurnValue < 0) {
        encTurnValue *= -1;
    }
    
    state->turnCount = encTurnValue / 2;  
    
    encTurnValue = encTurnValue % 2;
    
    // Button
    state->press = encPressed;
    state->pressCount = encPressedCount;
    
    // Clear
    ENC_Change = false;
}

/*******************************************************************************
 *          INTERRUPTS
 ******************************************************************************/
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void) {
    if (_T2IF) {
        _T2IF = 0;
        encode();
    }
}


