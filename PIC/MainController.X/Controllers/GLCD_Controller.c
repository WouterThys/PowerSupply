#include "GLCD_Controller.h"


static void uartReadDone(UartData_t data);

static GLCD_DataPacket_t glcdData[3];
static const size_t string_size = VALUE_LENGTH;
static char v_set_strings[3][VALUE_LENGTH] = { {0}, {0}, {0} };
static char i_set_strings[3][VALUE_LENGTH] = { {0}, {0}, {0} };
static char v_rd_strings [3][VALUE_LENGTH] = { {0}, {0}, {0} };
static char i_rd_strings [3][VALUE_LENGTH] = { {0}, {0}, {0} };

void glcdInit() {
    // Init uart
    uartDriverInit(UART1_BAUD, uartReadDone);
    uartDriverEnable(true);
    
    // Init Data Packet
    uint8_t i = 0;
    for (i = 0; i < 3; i++) {
        glcdData[i].command.menu = i;    
        glcdData[i].v_set = v_set_strings[i];
        glcdData[i].i_set = i_set_strings[i];
        glcdData[i].v_rd  = v_rd_strings[i];
        glcdData[i].i_rd  = i_rd_strings[i];
    }    
}

void glcdWriteMenu(const uint8_t menu) {
    uartDriverWriteByte(UART_START_CHAR);
    uartDriverWriteByte(glcdData[menu].command.cmd_bits);

    // Write string if needed
    switch(glcdData[menu].command.command) {
        case CMD_SET_V_SET:
            uartDriverWriteString(glcdData[menu].v_set, VALUE_LENGTH);
            break;
        case CMD_SET_I_SET:
            uartDriverWriteString(glcdData[menu].i_set, VALUE_LENGTH);
            break;
        case CMD_SET_V_READ:
            uartDriverWriteString(glcdData[menu].v_rd, VALUE_LENGTH);
            break;
        case CMD_SET_I_READ:
            uartDriverWriteString(glcdData[menu].i_rd, VALUE_LENGTH);
            break;
        case CMD_SET_A_READ:
            uartDriverWriteString(glcdData[menu].v_rd, VALUE_LENGTH);
            uartDriverWriteString(glcdData[menu].i_rd, VALUE_LENGTH);
            break;
    }

    // Clear command to not write same string again
    glcdData[menu].command.command = CMD_NONE;
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
    // Copy safely into string buffer
    strncpy(v_set_strings[menu], v, string_size);
    glcdData[menu].command.command = CMD_SET_V_SET;
}

void glcdSetCurrentSet(const uint8_t menu, const char * i) {
     // Copy safely into string buffer
    strncpy(i_set_strings[menu], i, string_size);
    glcdData[menu].command.command = CMD_SET_I_SET;
}

void glcdSetVoltageRead(const uint8_t menu, const char * v) {
     // Copy safely into string buffer
    strncpy(v_rd_strings[menu], v, string_size);
    if (glcdData[menu].command.command == CMD_SET_I_READ) {
        glcdData[menu].command.command = CMD_SET_A_READ;
    } else {
        glcdData[menu].command.command = CMD_SET_V_READ;
    }
}

void glcdSetCurrentRead(const uint8_t menu, const char * i) {
     // Copy safely into string buffer
    strncpy(i_rd_strings[menu], i, string_size);
    if (glcdData[menu].command.command == CMD_SET_V_READ) {
        glcdData[menu].command.command = CMD_SET_A_READ;
    } else {
        glcdData[menu].command.command = CMD_SET_I_READ;
    }
}

static void uartReadDone(UartData_t data) {

}
