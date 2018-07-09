#ifndef MENU_H
#define	MENU_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <xc.h> // include processor files - each processor file is guarded.  

#include "../../Common/COM_Settings.h"
#include "../Settings.h"
#include "../Drivers/SYSTEM_Driver.h"
#include "../Drivers/LCD_Driver.h"
#include "../Drivers/ENC_Driver.h"
#include "SUPPLIES_Controller.h"
    
    typedef struct {
        uint16_t changed : 1;
        uint16_t on : 1;
        uint16_t contrast : 6;      // LCD contrast from 1 -> 50
        uint16_t brightness : 4;    // LCD brightness from 1 -> 8
    } LCD_Settings_t;

    /**
     * 
     */
    void menuInit(bool (*onPutCommand)(Command_t data));
    
    /**
     * 
     */
    void menuUpdate(LCD_Settings_t settings, SupplyData_t dataVar);
    
    /**
     * 
     * @param voltage
     */
    void menuSetVariableVoltage(uint16_t voltage);
    
    /**
     * 
     * @param current
     */
    void menuSetVariableCurrent(uint16_t current);
    
    /**
     * 
     * @param voltage
     */
    void menuSetMeasuredVoltage(uint16_t voltage);
    
    /**
     * 
     * @param current
     */
    void menuSetMeasuredCurrent(uint16_t current);
    
    
    /**
     * 
     * @param on
     */
    void menuTurnOnCursor(bool on);
    
    
    
    
    /**
     * NEW NEW NEW
     */
    
    /**
     * 
     * @param msrVoltage
     * @param msrCurrent
     * @param msrTemperature
     */
    void menuUpdateMeasuredData(uint16_t msrVoltage, uint16_t msrCurrent, uint16_t msrTemperature);
    
    /**
     * 
     * @param selVoltage
     */
    void menuSelectVoltage(uint16_t selVoltage);
    
    /**
     * 
     * @param selCurrent
     */
    void menuSelectCurrent(uint16_t selCurrent);
    
    /**
     * 
     * @param selVoltage
     */
    void menuChangeVoltage(uint16_t selVoltage);
    
    /**
     * 
     * @param selCurrent
     */
    void menuChangeCurrent(uint16_t selCurrent);

#ifdef	__cplusplus
}
#endif

#endif	/* MENU_H */

