/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include <xc.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "GLCD_ks0108.h"
#include "UserFunctions.h"



/******************************************************************************/
/* GLCD Functions                                                             */
/******************************************************************************/
void DoSomething(uint8_t * r){
    GLCD_GotoXY(125,30);
    GLCD_DrawCircle(60,30,*r,WHITE);
}

void ResetSystem(uint8_t *r){
//    Delay1KTCYx(*r);
//    asm("RESET");
}
