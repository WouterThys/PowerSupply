/* 
 * File:   interrupts.h
 * Author: wouter
 *
 * Created on 3 maart 2015, 13:06
 */

#ifndef INT_DRIVER_H
#define	INT_DRIVER_H

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * Initializes the interrupts to the default settings.
 */
void intInit(void);
/**
 * Enable interrupts.
 */
void intEnable(bool enable);

/**
 * Enables interrupts for I2C messaging.
 * @param enable
 */
void intEnableI2CInterrupts(bool enable);


#ifdef	__cplusplus
}
#endif

#endif	