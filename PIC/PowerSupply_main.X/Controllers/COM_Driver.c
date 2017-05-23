#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>

#include "../Settings.h"

#include "COM_Driver.h"

/*******************************************************************************
 *          DEFINES
 ******************************************************************************/


/*******************************************************************************
 *          MACRO FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          VARIABLES
 ******************************************************************************/
static i2cData_t uartData;
static i2cData_t variableData;
static i2cData_t lcdData;



/*******************************************************************************
 *          LOCAL FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *          DRIVER FUNCTIONS
 ******************************************************************************/

void C_COM_Init() {
    // I2C
    D_I2C_InitMaster();
    uartData.address = I2C_UART_ADDRESS;
    variableData.address = I2C_VARIABLE_ADDRESS;
    lcdData.address = I2C_LCD_ADDRESS;
}

void C_COM_Enable(bool enable) {
    D_I2C_Enable(enable);
}

void C_COM_I2CReset() {
    D_I2C_Reset();
}

int16_t C_COM_LcdDraw(uint8_t menuId, uint8_t subMenuId) {
    uint8_t command = (COM_LCD_COM_DRAW << 6) & 0xC0;
    command |= ((menuId << 3) & 0x38);
    command |= (subMenuId & 0x07);
    
    lcdData.command = command;
    return D_I2C_MasterRead(&lcdData); // Write command, and read back status
}

int16_t C_COM_LcdSelect(uint8_t what, bool select, uint8_t id) {
    uint8_t command = (COM_LCD_COM_SELECT << 6) & 0xC0;
    command |= ((what << 5) & 0x20);
    command |= ((select << 4) & 0x10);
    command |= (id & 0x0F);
    
    lcdData.command = command;
    return D_I2C_MasterRead(&lcdData); // Write command, and read back status
}

int16_t C_COM_LcdSet(uint8_t id, int16_t value) {
    uint8_t command = (COM_LCD_COM_SET << 6) & 0xC0;
    command |= (id & 0x0F);
    
    lcdData.command = command;
    lcdData.data1 = (value & 0x00FF);
    lcdData.data2 = ((value >> 8) & 0x00FF);
    
    return D_I2C_MasterWrite(&lcdData); // Write command and data
}


/*******************************************************************************
 *          INTERRUPTS
 ******************************************************************************/


