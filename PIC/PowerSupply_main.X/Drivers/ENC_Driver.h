/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef ENC_DRIVER_H
#define	ENC_DRIVER_H

#include <xc.h> // include processor files - each processor file is guarded.  

typedef enum {
    NONE,
    LEFT,
    RIGHT
} encTurn_t;

typedef enum {
    PRESS, 
    NO_PRESS
} encPress_t;


extern bool ENC_Change;     /* Encoder value changed                          */

typedef struct {
    encTurn_t turn;         /* encTurn_t indicating turn direction LEFT-RIGHT or NONE */
    uint16_t turnCount;     /* Amount of turns in "turn" direction            */
    encPress_t press;       /* encPress_t indicating PRESS or NO_PRESS        */
    uint16_t pressCount;    /* Time of the press                              */
} enc_t;


/**
 * 
 */
void D_ENC_Init();

/**
 * 
 * @param enable
 */
void D_ENC_Enable(bool enable);

/**
 * 
 * @param state
 */
void D_ENC_GetState(enc_t *state);

#endif	/* XC_HEADER_TEMPLATE_H */

