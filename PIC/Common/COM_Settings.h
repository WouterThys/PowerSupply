/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef COM_SETTINGS_H
#define	COM_SETTINGS_H

#include <xc.h>
#include <stdint.h>

typedef struct {
    uint16_t command;
    uint16_t data;
} Command_t;

// Addresses
#define ADDR_MAIN        0x01
#define ADDR_VAR         0x55
#define ADDR_5V0         0x56
#define ADDR_3V3         0x57


// Main commands 0x0X
#define C_START             0x01
#define C_HANDSHAKE         0x02

// Supply commands 0x1X
#define C_SET_VOLTAGE       0x10
#define C_SET_CURRENT       0x11
#define C_GET_VOLTAGE       0x12
#define C_GET_CURRENT       0x13

// LCD commands 0x1X
#define C_LCD_BRIGHTNESS    0x70
#define C_LCD_CONTRAST      0x71


// I²C
#define COM_GET_I   0x01    
#define COM_GET_V   0x02
#define COM_GET_T   0x03
#define COM_A3      0x04


#define COM_SET_V   0x08
#define COM_SET_I   0x09





#endif	/* XC_HEADER_TEMPLATE_H */

