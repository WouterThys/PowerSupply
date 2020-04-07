#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <string.h>
#include <math.h>

#include "settings.h"
#include "Drivers/GLCD_bitmaps.h"
#include "Drivers/GLCD_ks0108.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/


/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize();

static void initialize() {

    // PORTS
    TRISA = 0x00;
    PORTA = 0x00;
    TRISB = 0x00;
    PORTB = 0x00;
    TRISC = 0x00;
    PORTC = 0x00;
    TRISD = 0x00;
    PORTD = 0x00;
    TRISE = 0x00;
    PORTE = 0x00;
    ADCON1 = 0x0F;
}

/*******************************************************************************
 *          MAIN PROGRAM
 ******************************************************************************/
int main(void) {

    initialize();

    GLCD_Init(NON_INVERTED);
    GLCD_SelectFont(font);
    GLCD_ClearScreen(BLACK);
//    GLCD_DrawBitmap(panda_bmp, 0, 0, WHITE);
//    GLCD_DrawRoundRect(0, 0, 191, 63, 8, WHITE);
    
    GLCD_GotoXY(0, 0);
    GLCD_Puts("Fuck you");
    


    while (1) {


        
    }
    return 0;
}
