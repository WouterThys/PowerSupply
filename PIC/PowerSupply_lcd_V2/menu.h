/* 
 * File:   menu.h
 * Author: wouter
 *
 * Created on February 17, 2018, 5:02 PM
 */

#ifndef MENU_H
#define	MENU_H

#ifdef	__cplusplus
extern "C" {
#endif
    
    
 /**
 * Definitions
 */
    
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
    

    void menuInit();
    
    void menuDrawMenu(uint16_t id);
    
    void menuDrawSubMenu(uint16_t id);
    
    void menuSetSelected(uint16_t what, uint16_t id, bool select);
    
    void menuSetFieldValue(uint16_t id, int16_t value);


#ifdef	__cplusplus
}
#endif

#endif	/* MENU_H */

