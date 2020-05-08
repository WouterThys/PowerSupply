#ifndef ENC_DRIVER_H
#define	ENC_DRIVER_H

#include <xc.h>
#include <stdint.h>
#include "../Settings.h"

typedef enum {
    Released = 0,
    Pressed = 1
} ButtonState_t;

typedef struct {
    ButtonState_t button;   // Button pressed or not
    int16_t turns;          // Turns, left or right
} Rotary_t;

#define ROTARY1 0
#define ROTARY2 1
#define ROTARY3 2

/**
 * Initialize rotary encoder
 * Uses interrupts on rotary port
 */
void encDriverInit();

/**
 * Get the value of the rotary encoder
 * @param rotary Which rotary
 * @param data Copy data into this
 */
void encGetRotaryData(uint16_t rotary, Rotary_t * data);



#endif	/* ENC_DRIVER_H */

