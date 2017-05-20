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

typedef struct MenuField menuField_t;
typedef struct SubMenu subMenu_t;
typedef struct Menu menu_t;


struct MenuField {
    uint8_t     id;
    char        *name;
    char        *units;
    double      value;
    uint8_t     line;
    bool        selectable;
    bool        selected;
    menuField_t *nextField;
};

struct SubMenu {
    uint8_t     id;       
    char  *name;    
    menuField_t *field1;
    menuField_t *field2;
    menuField_t *selectedField;
    bool        selected;
    subMenu_t   *nexSubMenu; 
};

struct Menu {
    uint8_t     id;       /* Unique id of the menu                            */
    char  *name;  /* Name of the menu (max 3 characters               */
    subMenu_t   *subMenu1; /* Sub menu 1                                       */
    subMenu_t   *subMenu2; /* Sub menu 2                                       */
    subMenu_t   *selectedSubMenu;
    bool        selected; /* Indicates if next menu indicator is selected     */
    menu_t      *nextMenu; /* Next menu                                        */
};


#endif	/* XC_HEADER_TEMPLATE_H */

