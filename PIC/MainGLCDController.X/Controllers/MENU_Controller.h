/* 
 * File:   MENU_Controller.h
 * Author: wouter
 *
 * Created on April 4, 2020, 6:46 PM
 */

#ifndef MENU_CONTROLLER_H
#define	MENU_CONTROLLER_H

#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "../Drivers/GLK19264A_Driver.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/ 
#define MENU_1       0
#define MENU_2       1
#define MENU_3       2

#define MENU_1_V     0
#define MENU_2_V     1
#define MENU_3_V     2

#define MENU_1_I     0
#define MENU_2_I     1
#define MENU_3_I     2

#define STATE_NONE   0
#define STATE_POINT  1
#define STATE_SELECT 2
    
/******************************************************************************/
/* Variables                                                                  */
/******************************************************************************/    
static const char * menu_1_title = "SUP 1";
static const char * menu_2_title = "SUP 2";
static const char * menu_3_title = "SUP 3";

static const char * menu_setup = "setup:";
static const char * menu_voltage_is = "V=";
static const char * menu_current_is = "I=";
static const char * menu_voltage_read = "V:";
static const char * menu_current_read = "I:";
static const char * menu_current_lim = "Ilim";
    
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

/**
 * Draw initial menu
 */
void menuInit();

/**
 * Select current menu
 * @param menu
 */
void menuSelect(const uint8_t menu, bool selected);

/**
 * Set the state
 */
void menuSetVoltageState(const uint8_t menu, const uint8_t state);


/**
 * Set the state
 */
void menuSetCurrentState(const uint8_t menu, const uint8_t state);

 /**
 * Set the voltage setting
 */
void menuSetVoltageSet(const uint8_t menu, const char * v);

/**
 * Set the current setting
 */
void menuSetCurrentSet(const uint8_t menu, const char * i);

/**
 * Set the voltage reading
 */
void menuSetVoltageRead(const uint8_t menu, const char * v);

/**
 * Set the current reading
 */
void menuSetCurrentRead(const uint8_t menu, const char * i);

#endif	/* MENU_CONTROLLER_H */

