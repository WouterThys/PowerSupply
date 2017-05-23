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
typedef enum Type type_t;
typedef enum Action action_t;

typedef struct Next next_t;

typedef struct Arrow arrow_t;
typedef struct Field field_t;
typedef struct SubMenu subMenu_t;
typedef struct Menu menu_t;
typedef struct Cursor cursor_t;

enum Selected {
    NS,
    CURSOR,
    SELECTED,
    NOT_SELECTABLE
};

enum Type {
    NOTHING,
    ARROW,
    FIELD,
    SUBMENU,
    MENU
};

enum Action {
    NA,
    DRAW,
    SELECT,
    SET
};

struct Next {
    type_t type;            /* Type of the next thing: ARROW - FIELD - SUB-MENU - MENU */
    int8_t id;              /* Id of the next item                            */
};

struct Arrow {
    int8_t      id;         /* Unique id                                      */
    selected_t  selected;   /* Selected state: NS - CURSOR - SELECTED         */
    next_t      next;       /* Next item to select                            */
    next_t      previous;   /* Previous arrow of field to select              */
};

struct Field {
    int8_t      id;         /* Unique id                                      */
    int16_t     value;      /* Value of the field                             */
    selected_t  selected;   /* Selected state: NS - CURSOR - SELECTED         */
    next_t      next;       /* Next item to select                            */
    next_t      previous;   /* Previous arrow of field to select              */
};

struct SubMenu {
    int8_t      id;         /* Unique id                                      */
    int8_t      field1Id;   /* Id of first field                              */
    int8_t      field2Id;   /* Id of second field                             */
    int8_t      arrowId;    /* Id of the arrow                                */
    int8_t      nextSubMenuId; /* Id of the next sub menu                     */
};

struct Menu {
    int8_t      id;         /* Unique id                                      */
    int8_t      subMenu1Id;
    int8_t      subMenu2Id;
    int8_t      arrowId;
    int8_t      nextMenuId;
};

struct Cursor {
    int8_t      currentId;
    type_t      currentType;
    union {
        arrow_t     *arrow;
        field_t     *field;
    } current;
    type_t      previousType;
    union {
        arrow_t     *arrow;
        field_t     *field;
    } previous;
    menu_t      *currentMenu;
    subMenu_t   *currentSubMenu;
    action_t    nextActions[4];
    uint8_t     nextActionCnt;
};

#endif	/* XC_HEADER_TEMPLATE_H */
