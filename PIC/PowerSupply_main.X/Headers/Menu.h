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

typedef enum Selected selected_t;
typedef enum NextType nextType_t;

typedef struct Arrow arrow_t;
typedef struct Field field_t;
typedef struct SubMenu subMenu_t;
typedef struct Menu menu_t;
typedef struct Next next_t;

enum Selected {
    NO,
    CURSOR,
    SELECTED
};

enum NextType {
    NOPE,
    MENU,
    SUBMENU,
    FIELD,
    ARROW
};

struct Next {
    nextType_t  nextType;   /* Type of the next thing                         */
    union {
        struct Menu      *nextMenu;
        struct SubMenu   *nextSubMenu;
        struct Field     *nextField;
        struct Arrow     *nextArrow;
    } next;
};

struct Arrow {
    uint8_t     id;         /* Unique id                                      */
    selected_t  selected;   /* Indicates if cursor is here and/or selected    */
    next_t      next;
    
};

struct Field {
    uint8_t     id;         /* Unique id                                      */
    int16_t     value;      /* Value of the field                             */
    selected_t  selected;   /* Indicates if cursor is here and/or selected    */
    next_t      next;
};

struct SubMenu {
    uint8_t     id;         /* Unique id                                      */
    field_t     *field;     /* Current field                                  */
};

struct Menu {
    uint8_t     id;         /* Unique id of the menu                          */
    subMenu_t   *subMenu;   /* Current sub-menu                               */
    next_t      next;
};

arrow_t varMnAr;
arrow_t sm1Ar;
arrow_t sm2Ar;
    
field_t voltageFld;
field_t currentFld;
field_t loadCurrentFld;
field_t temperatureFld;
    
subMenu_t varSm1;
subMenu_t varSm2;
    
menu_t varMn;


///**
// * Initial values
// */
//// Arrows 
//#define M0_SM0_AR   {ID_ARROW_SM_R, NO, &varSm2, SUBMENU}  
//#define M0_SM1_AR   {ID_ARROW_SM_L, NO, &varSm1, SUBMENU}  
//#define M0_AR       {ID_ARROW_M_R,  NO, NULL,    NOPE}  
//
//
//// Fields:           id,            value, selected, next
//#define M0_SM0_F0   {ID_M0_SM0_F0,  0,     NO,    &currentFld, FIELD} 
//#define M0_SM0_F1   {ID_M0_SM0_F1,  0,     NO,    &sm1Ar,      ARROW}
//#define M0_SM1_F0   {ID_M0_SM1_F0,  0,     NO,    NULL,        NOPE} 
//#define M0_SM1_F1   {ID_M0_SM1_F1,  0,     NO,    NULL,        NOPE} 
//
//// Sub menus:    id,         field,           next
//#define M0_SM0  {ID_M0_SM0,  &voltageField,   &varSm2, SUBMENU}
//#define M0_SM1  {ID_M0_SM1,  &temperatureFld, &varSm1, SUBMENU}
//
//// Menus:    id,     subMenu, next
//#define M0  {ID_M0,  &varSm1, &voltageField, FIELD}
//
//
///**
// * Assign
// */
//
//    varMnAr = M0_AR;
//    sm1Ar   = M0_SM0_AR;
//    sm2Ar   = M0_SM1_AR;
//    
//    voltageFld = M0_SM0_F0;
//    currentFld = M0_SM0_F1;
//    loadCurrentFld = M0_SM1_F0;
//    temperatureFld = M0_SM1_F1;
//    
//    varSm1 = M0_SM0;
//    varSm2 = M0_SM1;
//    
//    varMn = M0;

#endif	/* XC_HEADER_TEMPLATE_H */
