/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

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
    double      value;      /* Value of the field                             */
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


/**
 * Initial values
 */
// Arrows 
#define ARROW_M_L   {ID_ARROW_M_L,  127, 1, 1}  /* Left arrow for menu types  */     
#define ARROW_M_R   {ID_ARROW_M_R,  126, 1, 1}  /* Left arrow for menu types  */     
#define ARROW_SM_L  {ID_ARROW_SM_L, 127, 0, 15} /* Left arrow for menu types  */     
#define ARROW_SM_R  {ID_ARROW_SM_R, 126, 0, 15} /* Left arrow for menu types  */     

// Units
#define VOLTAGE     {ID_VOLTAGE,     "V", "mV"}
#define CURRENT     {ID_CURRENT,     "I", "mA"}
#define TEMPERATURE {ID_TEMPERATURE, "T", "°C"}

// Fields:           id,            units,       value, line, position
#define M0_SM0_F0   {ID_M0_SM0_F0,  VOLTAGE,     0.0,   0,    11} 
#define M0_SM0_F1   {ID_M0_SM0_F1,  CURRENT,     0.0,   1,    11}
#define M0_SM1_F0   {ID_M0_SM1_F0,  TEMPERATURE, 0.0,   0,    11} 
#define M0_SM1_F1   {ID_M0_SM1_F1,  CURRENT,     0.0,   1,    11} 

// Sub menus:    id,         name, field1,           field2,          arrow      
#define M0_SM0  {ID_M0_SM0,  "",   &voltageFld,      &currentFld,     &arrowSMenuR}
#define M0_SM1  {ID_M0_SM1,  "",   &loadCurrentFld,  &temperatureFld, &arrowSMenuL}

// Menus:    id,     name,  arrow
#define M0  {ID_M0,  "VAR", &arrowMenuR}


/**
 * Assign
 */
arrow_t arrowMenuL = ARROW_M_L;
arrow_t arrowMenuR = ARROW_M_R;
arrow_t arrowSMenuL = ARROW_SM_L;
arrow_t arrowSMenuR = ARROW_SM_R;
    
unit_t voltage = VOLTAGE;
unit_t current = CURRENT;
unit_t temperature = TEMPERATURE;
    
field_t voltageFld = M0_SM0_F0;
field_t currentFld = M0_SM0_F1;
field_t loadCurrentFld = M0_SM1_F0;
field_t temperatureFld = M0_SM1_F1;
    
subMenu_t varSm1 = M0_SM0;
subMenu_t varSm2 = M0_SM1;
    
menu_t varMn = M0;

#endif	/* XC_HEADER_TEMPLATE_H */

