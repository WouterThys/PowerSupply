
#ifndef MENU_H
#define	MENU_H

#ifdef	__cplusplus
extern "C" {
#endif

  

    void menuInit();
    
    void menuTurn(int16_t positions);
    
    void menuClicked();
    
    
    void menuGetVoltage(uint16_t * voltage);
    void menuGetCurrent(uint16_t * current);
    
    void menuSetMeasuredVoltage(uint16_t voltage);
    void menuSetMeasuredCurrent(uint16_t current);
    
    void menuTurnOnCursor(bool on);


#ifdef	__cplusplus
}
#endif

#endif	/* MENU_H */

