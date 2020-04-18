#include "GLCD_Controller.h"


static void uartReadDone(UartData_t data);

GLCD_DataPacket_t glcdData[3];


void glcdInit() {
    // Init uart
    uartDriverInit(UART1_BAUD, uartReadDone);
    uartDriverEnable(true);
    // 

}

void glcdWriteMenu(const uint8_t menu) {
    uartDriverWriteByte(UART_START_CHAR);
    DelayUs(1);
    uartDriverWriteByte(glcdData[menu].command.cmd_bits);
}

void glcdSelectMenu(const uint8_t menu, bool selected) {
    glcdData[menu].command.selected = selected;
}

void glcdSetVoltageState(const uint8_t menu, uint8_t state) {
     glcdData[menu].command.pointSelect = state;
}

void glcdSetCurrentState(const uint8_t menu, uint8_t state) {
     glcdData[menu].command.pointSelect = state;
}

void glcdSetVoltageSet(const uint8_t menu, const char * v) {
    uint8_t i = 0;
    for(i = 0; i < VALUE_LENGTH; i++) {
        *(glcdData[menu].v_set + i) = *(v + i);
    }
}

void glcdSetCurrentSet(const uint8_t menu, const char * i) {
     /* glcdData[menu].command.pointSelect = state; */
}

void glcdSetVoltageRead(const uint8_t menu, const char * v) {
     /* glcdData[menu].command.pointSelect = state; */
}

void glcdSetCurrentRead(const uint8_t menu, const char * i) {
     /* glcdData[menu].command.pointSelect = state; */
}

static void uartReadDone(UartData_t data) {

}
