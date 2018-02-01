/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef MENU_SETTINGS_H
#define	MENU_SETTINGS_H

/**
 * Field value types
 */
typedef enum {
    OUTPUT_VOLTAGE,
    CURRENT_LIMIT,
    CURRENT_LOAD,
    TEMPERATURE
} fieldType_t;

/**
 * Type definitions
 */
#define TYPE_ARROW      0
#define TYPE_FIELD      1

/**
 * Menu, SubMenu, Field, Unit and Arrow IDs
 */
#define ID_VOLTAGE      0
#define ID_CURRENT      1
#define ID_TEMPERATURE  2

#define ID_M0_SM0_AR    0
#define ID_M0_SM1_AR    1
#define ID_M0_AR        2

#define ID_M0_SM0_F0    0
#define ID_M0_SM0_F1    1
#define ID_M0_SM1_F0    2
#define ID_M0_SM1_F1    3

#define ID_M0_SM0       0   
#define ID_M0_SM1       1   

#define ID_M0           0


#endif	/* XC_HEADER_TEMPLATE_H */