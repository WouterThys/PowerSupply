/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef MENU_H
#define	MENU_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "../../Common/MENU_Settings.h"

/**
 * Definitions
 */

typedef struct Arrow arrow_t;
typedef struct Unit unit_t;
typedef struct Field field_t;
typedef struct SubMenu subMenu_t;
typedef struct Menu menu_t;

struct Arrow {
    uint8_t     id;         /* Unique id                                      */
    char        arrow;      /* Arrow type                                     */
    uint8_t     line;       /* Line where arrow is drawn on the screen        */
    uint8_t     pos;        /* Position where arrow is drawn on the screen    */
};

struct Unit {
    uint8_t     id;         /* Unique id                                      */
    char        *symbol;    /* Symbol of the unit                             */
    char        *units;     /* Units                                          */
};

struct Field {
    uint8_t     id;         /* Unique id                                      */
    unit_t      unit;       /* Units of the field                             */
    int16_t     value;      /* Value of the field                             */
    uint8_t     line;       /* Line where field is written on the screen      */
    uint8_t     pos;        /* Position of last digit on screen               */
};

struct SubMenu {
    uint8_t     id;         /* Unique id                                      */
    char        *name;      /* Name of the sub-menu                           */
    field_t     *field1;    /* First field on line 0                          */
    field_t     *field2;    /* Second field on line 1                         */
    arrow_t     *arrow;     /* Arrow of sub menu                              */
};

struct Menu {
    uint8_t     id;         /* Unique id of the menu                          */
    char        *name;      /* Name of the menu (max 3 characters             */
    arrow_t     *arrow;     /* Arrow of menu                                  */
    
};


#endif	/* XC_HEADER_TEMPLATE_H */

