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

    /**
     * 
     */
    void menuInit(bool (*onPutCommand)(Command_t data));
    
    /**
     * 
     * @param brightness
     * @param contrast
     */
    void menuConfigure(uint8_t brightness, uint8_t contrast);
    
    /**
     * 
     * @param msrVoltage
     * @param msrCurrent
     */
    void menuUpdateMeasured1Data(uint16_t msrVoltage, uint16_t msrCurrent, bool clip);
    
    /**
     * 
     * @param msrTemp
     * @param msrCurrent_
     */
    void menuUpdateMeasured2Data(uint16_t msrTemp, uint16_t msrCurrent_);
    
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
     */
    void menuSelectCalibration();
    
    /**
     * 
     */
    void menuSelectSettings();
    
    /**
     * 
     * @param brightness
     * @param contrast
     */
    void menuSelectBrightness(uint16_t brightness, uint16_t contrast);
    
    /**
     * 
     * @param brightness
     * @param contrast
     */
    void menuSelectContrast(uint16_t brightness, uint16_t contrast);
    
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

    /**
     * 
     */
    void menuChangeCalibration(uint16_t targetVoltage, uint16_t calibratedVoltage);
    
    /**
     * 
     */
    void menuChangeSettings();
    
        /**
     * 
     * @param brightness
     */
    void menuChangeBrightness(uint16_t brightness);
    
    /**
     * 
     * @param contrast
     */
    void menuChangeContrast(uint16_t contrast);
    
    /**
     * 
     * @param title
     * @param message
     */
    void menuShowError(const char * title, const char * message);
    
#ifdef	__cplusplus
}
#endif

#endif	/* MENU_H */

