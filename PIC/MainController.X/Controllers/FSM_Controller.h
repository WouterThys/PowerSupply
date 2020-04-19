/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef FSM_CONTROLLER_H_
#define	FSM_CONTROLLER_H_

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "../Settings.h"
#include "../Drivers/SYSTEM_Driver.h"
#include "../Drivers/ENC_Driver.h"
#include "../Controllers/GLCD_Controller.h"
#include "../Controllers/SUPPLIES_Controller.h"

/**
 * Initialize FSM Controller
 * Uses TIMER4
 **/
void fsmInit();

/**
 * Indicates the FSM should execute the next state
 **/
bool fsmShouldExecute();

/**
 *
 **/
void fsmExecute();

#endif
