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

#define UP      1
#define DOWN    2


extern bool ENC_Change;     /* Encoder value changed                          */

typedef struct {
    uint16_t turn;
    uint16_t turnCount;
    bool press;
    uint16_t pressCount;
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

