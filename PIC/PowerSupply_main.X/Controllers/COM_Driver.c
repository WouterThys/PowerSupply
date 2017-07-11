#include <xc.h>
#include <stdio.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>

#include "../Settings.h"
#include "../Drivers/SYSTEM_Driver.h"
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
static i2cData_t varData;
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
    varData.address = I2C_VARIABLE_ADDRESS;
    lcdData.address = I2C_LCD_ADDRESS;
}

void C_COM_Enable(bool enable) {
    D_I2C_Enable(enable);
}

void C_COM_I2CReset() {
    D_I2C_Reset();
}

int16_t C_COM_InitAll() {
    int16_t i2cResult = RES_NOK;
    uint8_t command = (COM_INIT << 5) & 0xE0;
    
    // Send to all devices
    varData.command = command;
    i2cResult = D_I2C_MasterRead(&varData);
    if (i2cResult < I2C_OK) return i2cResult;
    DelayUs(1);
    
    lcdData.command = command;
    i2cResult = D_I2C_MasterRead(&lcdData);
    if (i2cResult < I2C_OK) return i2cResult;
    
    // Wait for them to initialize
    DelayMs(1);
    
    // Check status of devices
    command = (COM_STATUS << 5) & 0xE0;
    
    varData.command = command;
    i2cResult = D_I2C_MasterRead(&varData);
    if (i2cResult < I2C_OK) return i2cResult;
    DelayUs(1);
    
    lcdData.command = command;
    i2cResult = D_I2C_MasterRead(&lcdData);
    if (i2cResult < I2C_OK) return i2cResult;
    
    if ((lcdData.status & STA_INITIALIZED_MSK) && (varData.status & STA_INITIALIZED_MSK)) {
        return RES_INIT_OK;
    } else {
        return RES_INIT_NOK;
    }
}

int16_t C_COM_EnableAll() {
    int16_t i2cResult = RES_NOK;
    uint8_t command = ((COM_ENABLE << 5) & 0xE0) + 1; // +1 to send enable
    
    // Send to all devices
    varData.command = command;
    i2cResult = D_I2C_MasterRead(&varData);
    if (i2cResult < I2C_OK) return i2cResult;
    DelayUs(1);
    
    lcdData.command = command;
    i2cResult = D_I2C_MasterRead(&lcdData);
    if (i2cResult < I2C_OK) return i2cResult;
    
    // Wait for them to initialize
    DelayMs(1);
    
    // Check status of devices
    command = (COM_STATUS << 5) & 0xE0;
    
    varData.command = command;
    i2cResult = D_I2C_MasterRead(&varData);
    if (i2cResult < I2C_OK) return i2cResult;
    DelayUs(1);
    
    lcdData.command = command;
    i2cResult = D_I2C_MasterRead(&lcdData);
    if (i2cResult < I2C_OK) return i2cResult;
    
    if ((lcdData.status & STA_ENABLED_MSK) && (varData.status & STA_ENABLED_MSK)) {
        return RES_ENA_OK;
    } else {
        return RES_ENA_NOK;
    }
}

int16_t C_COM_LcdStartActions() {
    uint8_t command = (COM_LCD << 5) & 0xE0;
    command |= COM_LCD_START;
    
    lcdData.command = command;
    return D_I2C_MasterRead(&lcdData);
}

int16_t C_COM_LcdStopActions() {
    uint8_t command = (COM_LCD << 5) & 0xE0;
    command |= COM_LCD_STOP;
    
    lcdData.command = command;
    return D_I2C_MasterRead(&lcdData);
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

int16_t C_COM_VarRead() {
    uint8_t command = (COM_READ << 5) & 0xE0;
    
    varData.command = command;
    return D_I2C_MasterRead(&varData);
}

int16_t C_COM_VarAsk(uint16_t bufferNr, int16_t * value) {
    int16_t result;
    uint8_t command = (COM_ASK << 5) & 0xE0;
    command |= (bufferNr & 0x0F);
    
    varData.command = command;
    result = D_I2C_MasterRead(&varData);
    if (result >= I2C_OK) {
        *value = ((varData.data1 << 8) & 0xFF00) | (varData.data2); 
    }
    return result;
}


/*******************************************************************************
 *          INTERRUPTS
 ******************************************************************************/


