#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <string.h>
#include <math.h>

#include "settings.h"
#include "Controllers/DATA_Controller.h"
#include "Controllers/MENU_Controller.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static GLCD_DataPacket_t data_packet;
static char v_set_arr[VALUE_LENGTH + 1];
static char i_set_arr[VALUE_LENGTH + 1];
static char v_rd_arr [VALUE_LENGTH + 1];
static char i_rd_arr [VALUE_LENGTH + 1];
static uint8_t dataReadFlag;

/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/
static void initialize();
static void onDataPacketRead(GLCD_DataPacket_t * data);
static void delaySeconds(uint8_t seconds);
static void uartDataRead(uint8_t data);

static void initialize() {
    // Disable interrupts
    GIEH = 0;
    GIEL = 0;

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
    dataInit(onDataPacketRead);
    menuInit();
    
    // Enable interrupts
    GIEH = 1;
    GIEL = 1;
    
    data_packet.command.cmd_bits = 0;
    data_packet.v_set = v_set_arr;
    data_packet.i_set = i_set_arr;
    data_packet.v_rd = v_rd_arr;
    data_packet.i_rd = i_rd_arr;
    
    while (1) {

        if (dataReadFlag) {
            LED1 = 1;
            dataReadFlag = 0;
    
            uint8_t menu = data_packet.command.menu;
            if (DEBUG) printf("cmd:%d\n", data_packet.command.cmd_bits);

            // Select or deselect menu
            menuSelect(menu, data_packet.command.selected);

            // Point and select
            switch (data_packet.command.pointSelect) {
                default:
                case POINT_TO_V:
                    menuSetCurrentState(menu, STATE_NONE);
                    menuSetVoltageState(menu, STATE_POINT);
                    break;
                case POINT_TO_I:
                    menuSetVoltageState(menu, STATE_NONE);
                    menuSetCurrentState(menu, STATE_POINT);
                    break;
                case SELECT_V:
                    menuSetCurrentState(menu, STATE_NONE);
                    menuSetVoltageState(menu, STATE_SELECT);
                    break;
                case SELECT_I:
                    menuSetVoltageState(menu, STATE_NONE);
                    menuSetCurrentState(menu, STATE_SELECT);
                    break;
            }

            // Set values
            switch (data_packet.command.command) {
                default:
                case CMD_NONE:
                    break;
                case CMD_SET_V_SET:
                    menuSetVoltageSet(menu, data_packet.v_set);
                    break;
                 case CMD_SET_I_SET:
                    menuSetCurrentSet(menu, data_packet.i_set);
                    break;
                case CMD_SET_V_READ:
                    menuSetVoltageRead(menu, data_packet.v_rd);
                    break;
                case CMD_SET_I_READ:
                    menuSetCurrentRead(menu, data_packet.i_rd);
                    break;
                case CMD_SET_A_READ:
                    menuSetVoltageRead(menu, data_packet.v_rd);
                    menuSetCurrentRead(menu, data_packet.i_rd);
                    break;
            }
            LED1 = 0;
        }

    }
    return 0;
}

static void uartDataRead(uint8_t data) {
    
}

static void onDataPacketRead(GLCD_DataPacket_t *data) {
    data_packet.command.cmd_bits = data->command.cmd_bits;
    strcpy(v_set_arr, data->v_set);
    strcpy(i_set_arr, data->i_set);
    strcpy(v_rd_arr,  data->v_rd);
    strcpy(i_rd_arr,  data->i_rd);
    dataReadFlag = 1; 
}

static void delaySeconds(uint8_t seconds) {
    uint8_t i, s;
    for (s = 0; s < seconds; s++) {
        for (i = 0; i < 100; i++) {
            __delay_ms(10);
        }
    }
    
}
