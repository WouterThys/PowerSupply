#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>

#include "../Settings.h"
#include "SYSTEM_Driver.h"
#include "ENC_Driver.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/
#define ALEFT   0x0001
#define BLEFT   0x0003
#define ARIGHT  0x0003
#define BRIGHT  0x0001

/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static uint16_t encA = 0;       /* Rotary line A                              */
static uint16_t encB = 0;       /* Rotary line B                              */
static uint16_t encAcnt = 0;    /* Counts turns in A direction                */
static uint16_t encBcnt = 0;    /* Counts turns in B direction                */
static uint16_t encCcnt = 0;    /* */

bool ENC_Change;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void encode();

void encode() {
    uint8_t A = ENC_Pin_1;
    uint8_t B = ENC_Pin_2;
    uint8_t C = ENC_Pin_3;

    if (C == 0) {
        if (ENC_Change == false) {
            ENC_Change = true;
        }
        encCcnt++;
    } else {

        encA <<= 1;
        encA |= A;

        encB <<= 1;
        encB |= B;

        encA &= 0x0003;
        encB &= 0x0003;

        // Compare with expected
        if (((encA == 0x0003) && (encB == 0x0001)) || ((encA == 0x0000) && (encB == 0x0002))) {
            ENC_Change = true;
            encAcnt++;
        }
        if (((encA == 0x0001) && (encB == 0x0003)) || ((encA == 0x0002) && (encB == 0x0000))) {
            ENC_Change = true;
            encBcnt++;
        }

    }
}

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/

void D_ENC_Init() {
    D_ENC_Enable(false);

    /* Interrupts */
    CNENBbits.CNIEB10 = 1;
    CNENBbits.CNIEB11 = 1;
    CNENBbits.CNIEB12 = 1;
    CNPUBbits.CNPUB10 = 1;
    CNPUBbits.CNPUB11 = 1;
    CNPUBbits.CNPUB12 = 1;

    _CNIF = 0; // Clear flag
    _CNIP = CN_IP;
    _CNIE = 0;

    /* Variables */
    ENC_Change = false;
}

void D_ENC_Enable(bool enable) {
    if (enable) {
        ENC_Dir_1 = 1; // Input
        ENC_Dir_2 = 1; // Input
        ENC_Dir_3 = 1; // Input
        ENC_Change = false;
        DelayMs(1);
        encA = encB = 0;
        _CNIE = 1;
    } else {
        ENC_Dir_1 = 0; // Output
        ENC_Dir_2 = 0; // Output
        ENC_Dir_3 = 0; // Output
        _CNIE = 0;
    }
}

void D_ENC_GetState(enc_t *state) {
    if (encAcnt > encBcnt) {
        state->turn = LEFT;
        state->turnCount = encAcnt - encBcnt;
    } else if (encAcnt < encBcnt) {
        state->turn = RIGHT;
        state->turnCount = encBcnt - encAcnt;
    } else {
        state->turn = NONE;
        state->turnCount = 0;
    }

    if (encCcnt > 0) {
        state->press = PRESS;
    } else {
        state->press = NO_PRESS;
    }
    state->pressCount = encCcnt;

    encAcnt = 0;
    encBcnt = 0;
    encCcnt = 0;
    ENC_Change = false;
}

/*******************************************************************************
 *          INTERRUPTS
 ******************************************************************************/
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void) {
    if (_CNIF) {
        encode();
        _CNIF = 0;
    }
}


